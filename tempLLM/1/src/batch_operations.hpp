#pragma once

#include "opengl_types.hpp"
#include "handles.hpp"
#include "functions.hpp"
#include "validation/validator.hpp"
#include <vector>
#include <memory>
#include <cstring>

namespace gl46::batch {

// Оптимизированный класс для пакетной загрузки данных в буфер
template<typename T>
class BatchBuffer {
private:
    BufferHandle handle_;
    size_t capacity_;
    size_t current_size_{0};
    BufferUsage usage_;
    
public:
    BatchBuffer(size_t initial_capacity, BufferUsage usage = BufferUsage::DynamicDraw) 
        : capacity_(initial_capacity), usage_(usage) {
        handle_ = CreateBuffer(BufferType::ArrayBuffer, capacity_ * sizeof(T), nullptr, usage);
        validation::Validator::ValidateHandle(handle_, "Buffer");
    }
    
    // Добавление одиночного элемента
    void Add(const T& element) {
        if (current_size_ >= capacity_) {
            Resize(capacity_ * 2);
        }
        
        SetBufferSubData(handle_, current_size_ * sizeof(T), sizeof(T), &element);
        ++current_size_;
        validation::Validator::CheckError("BatchBuffer::Add");
    }
    
    // Добавление массива элементов
    template<size_t N>
    void AddArray(const T (&data)[N]) {
        if ((current_size_ + N) > capacity_) {
            size_t new_capacity = std::max(capacity_ * 2, current_size_ + N);
            Resize(new_capacity);
        }
        
        SetBufferSubData(handle_, current_size_ * sizeof(T), N * sizeof(T), data);
        current_size_ += N;
        validation::Validator::CheckError("BatchBuffer::AddArray");
    }
    
    // Добавление вектора элементов
    void AddVector(const std::vector<T>& data) {
        if ((current_size_ + data.size()) > capacity_) {
            size_t new_capacity = std::max(capacity_ * 2, current_size_ + data.size());
            Resize(new_capacity);
        }
        
        if (!data.empty()) {
            SetBufferSubData(handle_, current_size_ * sizeof(T), data.size() * sizeof(T), data.data());
            current_size_ += data.size();
        }
        validation::Validator::CheckError("BatchBuffer::AddVector");
    }
    
    // Оптимизированное добавление с предварительной проверкой
    void AddSpan(const T* data, size_t count) {
        if (count == 0) return;
        
        if ((current_size_ + count) > capacity_) {
            size_t new_capacity = std::max(capacity_ * 2, current_size_ + count);
            Resize(new_capacity);
        }
        
        SetBufferSubData(handle_, current_size_ * sizeof(T), count * sizeof(T), data);
        current_size_ += count;
        validation::Validator::CheckError("BatchBuffer::AddSpan");
    }
    
    // Очистка буфера (сброс указателя, но без реаллокации)
    void Clear() {
        current_size_ = 0;
    }
    
    // Полная очистка с освобождением памяти
    void Reset(size_t new_capacity) {
        DestroyBuffer(handle_);
        capacity_ = new_capacity;
        current_size_ = 0;
        handle_ = CreateBuffer(BufferType::ArrayBuffer, capacity_ * sizeof(T), nullptr, usage_);
        validation::Validator::ValidateHandle(handle_, "Buffer");
    }
    
    // Получение хендла буфера для использования в OpenGL
    BufferHandle GetHandle() const { return handle_; }
    
    // Получение текущего размера
    size_t GetSize() const { return current_size_; }
    
    // Получение емкости
    size_t GetCapacity() const { return capacity_; }
    
    // Проверка на пустоту
    bool Empty() const { return current_size_ == 0; }
    
    // Заполнение буфера с использованием маппинга памяти (для лучшей производительности)
    void FillMapped(const std::vector<T>& data) {
        if (data.size() > capacity_) {
            Resize(std::max(capacity_ * 2, data.size()));
        }
        
        void* mapped_memory = MapBuffer(handle_, BufferAccess::WriteOnly);
        if (mapped_memory) {
            std::memcpy(mapped_memory, data.data(), data.size() * sizeof(T));
            UnmapBuffer(handle_);
            current_size_ = data.size();
        } else {
            // Резервный вариант через SubData
            SetBufferData(handle_, data.size() * sizeof(T), data.data(), usage_);
            current_size_ = data.size();
        }
        validation::Validator::CheckError("BatchBuffer::FillMapped");
    }
    
private:
    void Resize(size_t new_capacity) {
        BufferHandle new_buffer = CreateBuffer(
            BufferType::ArrayBuffer, 
            new_capacity * sizeof(T), 
            nullptr, 
            usage_
        );
        validation::Validator::ValidateHandle(new_buffer, "Buffer");
        
        // Копирование старых данных если они есть
        if (current_size_ > 0) {
            void* old_data = MapBuffer(handle_, BufferAccess::ReadOnly);
            if (old_data) {
                SetBufferData(new_buffer, current_size_ * sizeof(T), old_data, usage_);
                UnmapBuffer(handle_);
            } else {
                // Резервный вариант через GetBufferSubData если маппинг не удался
                std::vector<T> temp_data(current_size_);
                GetBufferSubData(handle_, 0, current_size_ * sizeof(T), temp_data.data());
                SetBufferData(new_buffer, current_size_ * sizeof(T), temp_data.data(), usage_);
            }
        }
        
        DestroyBuffer(handle_);
        handle_ = new_buffer;
        capacity_ = new_capacity;
        validation::Validator::CheckError("BatchBuffer::Resize");
    }
    
public:
    ~BatchBuffer() {
        if (handle_.valid()) {
            DestroyBuffer(handle_);
        }
    }
};

// Класс для пакетной отрисовки
class BatchRenderer {
private:
    std::vector<GLuint> draw_counts_;
    std::vector<GLint> draw_firsts_;
    std::vector<const void*> draw_indirects_;
    
public:
    // Добавление команды отрисовки в пакет
    void AddDrawCommand(GLint first, GLsizei count) {
        draw_firsts_.push_back(first);
        draw_counts_.push_back(count);
        // Для indirect отрисовки сохраняем указатель на структуру с параметрами
    }
    
    // Выполнение пакетной отрисовки
    void ExecuteDrawCommands(PrimitiveType mode) {
        if (!draw_counts_.empty()) {
            // Используем MultiDrawArrays для выполнения всех команд за один вызов
            glMultiDrawArrays(
                static_cast<GLenum>(mode),
                draw_firsts_.data(),
                draw_counts_.data(),
                static_cast<GLsizei>(draw_counts_.size())
            );
            validation::Validator::CheckError("BatchRenderer::ExecuteDrawCommands");
        }
    }
    
    // Очистка пакета
    void Clear() {
        draw_counts_.clear();
        draw_firsts_.clear();
        draw_indirects_.clear();
    }
    
    // Получение количества команд в пакете
    size_t GetCommandCount() const {
        return draw_counts_.size();
    }
};

// Класс для пакетной загрузки текстур
class TextureBatchLoader {
private:
    std::vector<TextureHandle> textures_;
    
public:
    // Загрузка текстуры в пакет
    TextureHandle AddTexture(
        TextureTarget target,
        GLsizei width,
        GLsizei height,
        TextureInternalFormat internal_format,
        TextureFormat format,
        TextureDataType type,
        const void* data
    ) {
        TextureHandle texture = CreateTexture(target);
        validation::Validator::ValidateHandle(texture, "Texture");
        
        SetTextureImage2D(texture, 0, internal_format, width, height, 0, format, type, data);
        SetTextureParameter(texture, TextureParameterName::WrapS, static_cast<GLint>(WrapMode::Repeat));
        SetTextureParameter(texture, TextureParameterName::WrapT, static_cast<GLint>(WrapMode::Repeat));
        SetTextureParameter(texture, TextureParameterName::MinFilter, static_cast<GLint>(FilterMode::Linear));
        SetTextureParameter(texture, TextureParameterName::MagFilter, static_cast<GLint>(FilterMode::Linear));
        
        textures_.push_back(texture);
        return texture;
    }
    
    // Очистка всех загруженных текстур
    void Clear() {
        for (auto texture : textures_) {
            if (texture.valid()) {
                DestroyTexture(texture);
            }
        }
        textures_.clear();
    }
    
    ~TextureBatchLoader() {
        Clear();
    }
};

} // namespace gl46::batch