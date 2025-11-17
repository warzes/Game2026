#pragma once

#include <cmath>
#include <algorithm>
#include <concepts>

// Определяем используемый тип данных
using real = float;

// Математические константы
namespace math_constants {
    constexpr real PI = 3.14159265358979323846f;
    constexpr real TWO_PI = 2.0f * PI;
    constexpr real HALF_PI = 0.5f * PI;
    constexpr real INV_PI = 1.0f / PI;
    constexpr real DEG_TO_RAD = PI / 180.0f;
    constexpr real RAD_TO_DEG = 180.0f / PI;
}

// Векторные классы
struct Vector2;
struct Vector3;
struct Vector4;

// Матричные классы
struct Matrix3x3;
struct Matrix4x4;

// Класс кватерниона
struct Quaternion;

// Базовый класс для векторов
template<size_t N>
struct VectorBase {
    real data[N];
    
    constexpr real& operator[](size_t i) { return data[i]; }
    constexpr const real& operator[](size_t i) const { return data[i]; }
    
    // Операторы сравнения
    constexpr bool operator==(const VectorBase& other) const {
        for(size_t i = 0; i < N; ++i) {
            if(data[i] != other.data[i]) return false;
        }
        return true;
    }
    
    constexpr bool operator!=(const VectorBase& other) const {
        return !(*this == other);
    }
};

// Vector2
struct Vector2 : public VectorBase<2> {
    // Конструкторы
    constexpr Vector2() : VectorBase<2>{{0.0f, 0.0f}} {}
    constexpr Vector2(real x, real y) : VectorBase<2>{{x, y}} {}
    constexpr explicit Vector2(real xy) : VectorBase<2>{{xy, xy}} {}
    
    // Получение компонентов
    constexpr real x() const { return data[0]; }
    constexpr real y() const { return data[1]; }
    
    // Операторы присваивания
    constexpr Vector2& operator+=(const Vector2& other) {
        data[0] += other.data[0];
        data[1] += other.data[1];
        return *this;
    }
    
    constexpr Vector2& operator-=(const Vector2& other) {
        data[0] -= other.data[0];
        data[1] -= other.data[1];
        return *this;
    }
    
    constexpr Vector2& operator*=(real scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        return *this;
    }
    
    constexpr Vector2& operator/=(real scalar) {
        data[0] /= scalar;
        data[1] /= scalar;
        return *this;
    }
    
    // Операторы унарного минуса
    constexpr Vector2 operator-() const {
        return Vector2{-data[0], -data[1]};
    }
    
    // Длина вектора
    constexpr real length() const {
        return std::sqrt(length_squared());
    }
    
    // Квадрат длины вектора
    constexpr real length_squared() const {
        return data[0]*data[0] + data[1]*data[1];
    }
    
    // Нормализация
    constexpr Vector2 normalized() const {
        real len = length();
        if(len > 0.0f) {
            return Vector2{data[0]/len, data[1]/len};
        }
        return Vector2{0.0f, 0.0f};
    }
    
    // Нормализация вектора
    constexpr void normalize() {
        *this = normalized();
    }
    
    // Скалярное произведение
    constexpr real dot(const Vector2& other) const {
        return data[0]*other.data[0] + data[1]*other.data[1];
    }
    
    // Перпендикулярный вектор (вращение на 90 градусов)
    constexpr Vector2 perpendicular() const {
        return Vector2{-data[1], data[0]};
    }
    
    // Линейная интерполяция
    constexpr Vector2 lerp(const Vector2& other, real t) const {
        return Vector2{
            data[0] + (other.data[0] - data[0]) * t,
            data[1] + (other.data[1] - data[1]) * t
        };
    }
};

// Операторы для Vector2
constexpr Vector2 operator+(const Vector2& a, const Vector2& b) {
    return Vector2{a.data[0] + b.data[0], a.data[1] + b.data[1]};
}

constexpr Vector2 operator-(const Vector2& a, const Vector2& b) {
    return Vector2{a.data[0] - b.data[0], a.data[1] - b.data[1]};
}

constexpr Vector2 operator*(const Vector2& v, real scalar) {
    return Vector2{v.data[0] * scalar, v.data[1] * scalar};
}

constexpr Vector2 operator*(real scalar, const Vector2& v) {
    return Vector2{v.data[0] * scalar, v.data[1] * scalar};
}

constexpr Vector2 operator/(const Vector2& v, real scalar) {
    return Vector2{v.data[0] / scalar, v.data[1] / scalar};
}

// Vector3
struct Vector3 : public VectorBase<3> {
    // Конструкторы
    constexpr Vector3() : VectorBase<3>{{0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3(real x, real y, real z) : VectorBase<3>{{x, y, z}} {}
    constexpr explicit Vector3(real xyz) : VectorBase<3>{{xyz, xyz, xyz}} {}
    constexpr Vector3(const Vector2& xy, real z) : VectorBase<3>{{xy.data[0], xy.data[1], z}} {}
    
    // Получение компонентов
    constexpr real x() const { return data[0]; }
    constexpr real y() const { return data[1]; }
    constexpr real z() const { return data[2]; }
    
    // Получение Vector2
    constexpr Vector2 xy() const { return Vector2{data[0], data[1]}; }
    
    // Операторы присваивания
    constexpr Vector3& operator+=(const Vector3& other) {
        data[0] += other.data[0];
        data[1] += other.data[1];
        data[2] += other.data[2];
        return *this;
    }
    
    constexpr Vector3& operator-=(const Vector3& other) {
        data[0] -= other.data[0];
        data[1] -= other.data[1];
        data[2] -= other.data[2];
        return *this;
    }
    
    constexpr Vector3& operator*=(real scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        data[2] *= scalar;
        return *this;
    }
    
    constexpr Vector3& operator/=(real scalar) {
        data[0] /= scalar;
        data[1] /= scalar;
        data[2] /= scalar;
        return *this;
    }
    
    // Оператор унарного минуса
    constexpr Vector3 operator-() const {
        return Vector3{-data[0], -data[1], -data[2]};
    }
    
    // Длина вектора
    constexpr real length() const {
        return std::sqrt(length_squared());
    }
    
    // Квадрат длины вектора
    constexpr real length_squared() const {
        return data[0]*data[0] + data[1]*data[1] + data[2]*data[2];
    }
    
    // Нормализация
    constexpr Vector3 normalized() const {
        real len = length();
        if(len > 0.0f) {
            return Vector3{data[0]/len, data[1]/len, data[2]/len};
        }
        return Vector3{0.0f, 0.0f, 0.0f};
    }
    
    // Нормализация вектора
    constexpr void normalize() {
        *this = normalized();
    }
    
    // Скалярное произведение
    constexpr real dot(const Vector3& other) const {
        return data[0]*other.data[0] + data[1]*other.data[1] + data[2]*other.data[2];
    }
    
    // Векторное произведение
    constexpr Vector3 cross(const Vector3& other) const {
        return Vector3{
            data[1]*other.data[2] - data[2]*other.data[1],
            data[2]*other.data[0] - data[0]*other.data[2],
            data[0]*other.data[1] - data[1]*other.data[0]
        };
    }
    
    // Линейная интерполяция
    constexpr Vector3 lerp(const Vector3& other, real t) const {
        return Vector3{
            data[0] + (other.data[0] - data[0]) * t,
            data[1] + (other.data[1] - data[1]) * t,
            data[2] + (other.data[2] - data[2]) * t
        };
    }
};

// Операторы для Vector3
constexpr Vector3 operator+(const Vector3& a, const Vector3& b) {
    return Vector3{a.data[0] + b.data[0], a.data[1] + b.data[1], a.data[2] + b.data[2]};
}

constexpr Vector3 operator-(const Vector3& a, const Vector3& b) {
    return Vector3{a.data[0] - b.data[0], a.data[1] - b.data[1], a.data[2] - b.data[2]};
}

constexpr Vector3 operator*(const Vector3& v, real scalar) {
    return Vector3{v.data[0] * scalar, v.data[1] * scalar, v.data[2] * scalar};
}

constexpr Vector3 operator*(real scalar, const Vector3& v) {
    return Vector3{v.data[0] * scalar, v.data[1] * scalar, v.data[2] * scalar};
}

constexpr Vector3 operator/(const Vector3& v, real scalar) {
    return Vector3{v.data[0] / scalar, v.data[1] / scalar, v.data[2] / scalar};
}

// Vector4
struct Vector4 : public VectorBase<4> {
    // Конструкторы
    constexpr Vector4() : VectorBase<4>{{0.0f, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector4(real x, real y, real z, real w) : VectorBase<4>{{x, y, z, w}} {}
    constexpr explicit Vector4(real xyzw) : VectorBase<4>{{xyzw, xyzw, xyzw}} {}
    constexpr Vector4(const Vector3& xyz, real w) : VectorBase<4>{{xyz.data[0], xyz.data[1], xyz.data[2], w}} {}
    constexpr Vector4(const Vector2& xy, real z, real w) : VectorBase<4>{{xy.data[0], xy.data[1], z, w}} {}
    
    // Получение компонентов
    constexpr real x() const { return data[0]; }
    constexpr real y() const { return data[1]; }
    constexpr real z() const { return data[2]; }
    constexpr real w() const { return data[3]; }
    
    // Получение Vector3
    constexpr Vector3 xyz() const { return Vector3{data[0], data[1], data[2]}; }
    
    // Получение Vector2
    constexpr Vector2 xy() const { return Vector2{data[0], data[1]}; }
    
    // Операторы присваивания
    constexpr Vector4& operator+=(const Vector4& other) {
        data[0] += other.data[0];
        data[1] += other.data[1];
        data[2] += other.data[2];
        data[3] += other.data[3];
        return *this;
    }
    
    constexpr Vector4& operator-=(const Vector4& other) {
        data[0] -= other.data[0];
        data[1] -= other.data[1];
        data[2] -= other.data[2];
        data[3] -= other.data[3];
        return *this;
    }
    
    constexpr Vector4& operator*=(real scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        data[2] *= scalar;
        data[3] *= scalar;
        return *this;
    }
    
    constexpr Vector4& operator/=(real scalar) {
        data[0] /= scalar;
        data[1] /= scalar;
        data[2] /= scalar;
        data[3] /= scalar;
        return *this;
    }
    
    // Оператор унарного минуса
    constexpr Vector4 operator-() const {
        return Vector4{-data[0], -data[1], -data[2], -data[3]};
    }
    
    // Длина вектора
    constexpr real length() const {
        return std::sqrt(length_squared());
    }
    
    // Квадрат длины вектора
    constexpr real length_squared() const {
        return data[0]*data[0] + data[1]*data[1] + data[2]*data[2] + data[3]*data[3];
    }
    
    // Нормализация
    constexpr Vector4 normalized() const {
        real len = length();
        if(len > 0.0f) {
            return Vector4{data[0]/len, data[1]/len, data[2]/len, data[3]/len};
        }
        return Vector4{0.0f, 0.0f, 0.0f, 0.0f};
    }
    
    // Нормализация вектора
    constexpr void normalize() {
        *this = normalized();
    }
    
    // Скалярное произведение
    constexpr real dot(const Vector4& other) const {
        return data[0]*other.data[0] + data[1]*other.data[1] + data[2]*other.data[2] + data[3]*other.data[3];
    }
    
    // Линейная интерполяция
    constexpr Vector4 lerp(const Vector4& other, real t) const {
        return Vector4{
            data[0] + (other.data[0] - data[0]) * t,
            data[1] + (other.data[1] - data[1]) * t,
            data[2] + (other.data[2] - data[2]) * t,
            data[3] + (other.data[3] - data[3]) * t
        };
    }
};

// Операторы для Vector4
constexpr Vector4 operator+(const Vector4& a, const Vector4& b) {
    return Vector4{a.data[0] + b.data[0], a.data[1] + b.data[1], a.data[2] + b.data[2], a.data[3] + b.data[3]};
}

constexpr Vector4 operator-(const Vector4& a, const Vector4& b) {
    return Vector4{a.data[0] - b.data[0], a.data[1] - b.data[1], a.data[2] - b.data[2], a.data[3] - b.data[3]};
}

constexpr Vector4 operator*(const Vector4& v, real scalar) {
    return Vector4{v.data[0] * scalar, v.data[1] * scalar, v.data[2] * scalar, v.data[3] * scalar};
}

constexpr Vector4 operator*(real scalar, const Vector4& v) {
    return Vector4{v.data[0] * scalar, v.data[1] * scalar, v.data[2] * scalar, v.data[3] * scalar};
}

constexpr Vector4 operator/(const Vector4& v, real scalar) {
    return Vector4{v.data[0] / scalar, v.data[1] / scalar, v.data[2] / scalar, v.data[3] / scalar};
}

// Матричные классы
template<size_t Rows, size_t Cols>
struct MatrixBase {
    real data[Rows][Cols];
    
    // Доступ к элементам
    constexpr real* operator[](size_t row) { return data[row]; }
    constexpr const real* operator[](size_t row) const { return data[row]; }
    
    // Операторы сравнения
    constexpr bool operator==(const MatrixBase& other) const {
        for(size_t i = 0; i < Rows; ++i) {
            for(size_t j = 0; j < Cols; ++j) {
                if(data[i][j] != other.data[i][j]) return false;
            }
        }
        return true;
    }
    
    constexpr bool operator!=(const MatrixBase& other) const {
        return !(*this == other);
    }
};

// Matrix3x3
struct Matrix3x3 : public MatrixBase<3, 3> {
    // Конструкторы
    constexpr Matrix3x3() : MatrixBase<3, 3>() {
        for(size_t i = 0; i < 3; ++i) {
            for(size_t j = 0; j < 3; ++j) {
                data[i][j] = (i == j) ? 1.0f : 0.0f;  // Единичная матрица
            }
        }
    }
    
    constexpr Matrix3x3(
        real m00, real m01, real m02,
        real m10, real m11, real m12,
        real m20, real m21, real m22
    ) : MatrixBase<3, 3>() {
        data[0][0] = m00; data[0][1] = m01; data[0][2] = m02;
        data[1][0] = m10; data[1][1] = m11; data[1][2] = m12;
        data[2][0] = m20; data[2][1] = m21; data[2][2] = m22;
    }
    
    // Создание из массива
    constexpr static Matrix3x3 from_array(const real arr[9]) {
        return Matrix3x3{
            arr[0], arr[1], arr[2],
            arr[3], arr[4], arr[5],
            arr[6], arr[7], arr[8]
        };
    }
    
    // Умножение матриц
    constexpr Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for(size_t i = 0; i < 3; ++i) {
            for(size_t j = 0; j < 3; ++j) {
                result.data[i][j] = 0.0f;
                for(size_t k = 0; k < 3; ++k) {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }
    
    // Умножение на вектор
    constexpr Vector3 operator*(const Vector3& v) const {
        return Vector3{
            data[0][0]*v.data[0] + data[0][1]*v.data[1] + data[0][2]*v.data[2],
            data[1][0]*v.data[0] + data[1][1]*v.data[1] + data[1][2]*v.data[2],
            data[2][0]*v.data[0] + data[2][1]*v.data[1] + data[2][2]*v.data[2]
        };
    }
    
    // Транспонирование
    constexpr Matrix3x3 transposed() const {
        return Matrix3x3{
            data[0][0], data[1][0], data[2][0],
            data[0][1], data[1][1], data[2][1],
            data[0][2], data[1][2], data[2][2]
        };
    }
    
    // Транспонирование матрицы
    constexpr void transpose() {
        *this = transposed();
    }
    
    // Определитель
    constexpr real determinant() const {
        return data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1])
             - data[0][1] * (data[1][0] * data[2][2] - data[1][2] * data[2][0])
             + data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
    }
    
    // Инверсия
    constexpr Matrix3x3 inverted() const {
        real det = determinant();
        if(det == 0.0f) return Matrix3x3{}; // Возвращаем единичную матрицу при нулевом детерминанте
        
        real inv_det = 1.0f / det;
        
        return Matrix3x3{
            inv_det * (data[1][1] * data[2][2] - data[1][2] * data[2][1]),
            inv_det * (data[0][2] * data[2][1] - data[0][1] * data[2][2]),
            inv_det * (data[0][1] * data[1][2] - data[0][2] * data[1][1]),
            
            inv_det * (data[1][2] * data[2][0] - data[1][0] * data[2][2]),
            inv_det * (data[0][0] * data[2][2] - data[0][2] * data[2][0]),
            inv_det * (data[0][2] * data[1][0] - data[0][0] * data[1][2]),
            
            inv_det * (data[1][0] * data[2][1] - data[1][1] * data[2][0]),
            inv_det * (data[0][1] * data[2][0] - data[0][0] * data[2][1]),
            inv_det * (data[0][0] * data[1][1] - data[0][1] * data[1][0])
        };
    }
    
    // Инвертирование матрицы
    constexpr void invert() {
        *this = inverted();
    }
    
    // Создание матрицы поворота вокруг оси Z
    constexpr static Matrix3x3 rotation_z(real angle) {
        real cos_a = std::cos(angle);
        real sin_a = std::sin(angle);
        
        return Matrix3x3{
            cos_a, -sin_a, 0.0f,
            sin_a,  cos_a, 0.0f,
            0.0f,   0.0f,  1.0f
        };
    }
    
    // Создание масштабирующей матрицы
    constexpr static Matrix3x3 scale(real sx, real sy, real sz) {
        return Matrix3x3{
            sx,   0.0f, 0.0f,
            0.0f, sy,   0.0f,
            0.0f, 0.0f, sz
        };
    }
    
    // Создание масштабирующей матрицы (одинаковый масштаб по всем осям)
    constexpr static Matrix3x3 scale(real s) {
        return scale(s, s, s);
    }
};

// Matrix4x4
struct Matrix4x4 : public MatrixBase<4, 4> {
    // Конструкторы
    constexpr Matrix4x4() : MatrixBase<4, 4>() {
        for(size_t i = 0; i < 4; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                data[i][j] = (i == j) ? 1.0f : 0.0f;  // Единичная матрица
            }
        }
    }
    
    constexpr Matrix4x4(
        real m00, real m01, real m02, real m03,
        real m10, real m11, real m12, real m13,
        real m20, real m21, real m22, real m23,
        real m30, real m31, real m32, real m33
    ) : MatrixBase<4, 4>() {
        data[0][0] = m00; data[0][1] = m01; data[0][2] = m02; data[0][3] = m03;
        data[1][0] = m10; data[1][1] = m11; data[1][2] = m12; data[1][3] = m13;
        data[2][0] = m20; data[2][1] = m21; data[2][2] = m22; data[2][3] = m23;
        data[3][0] = m30; data[3][1] = m31; data[3][2] = m32; data[3][3] = m33;
    }
    
    // Создание из массива
    constexpr static Matrix4x4 from_array(const real arr[16]) {
        return Matrix4x4{
            arr[0],  arr[1],  arr[2],  arr[3],
            arr[4],  arr[5],  arr[6],  arr[7],
            arr[8],  arr[9],  arr[10], arr[11],
            arr[12], arr[13], arr[14], arr[15]
        };
    }
    
    // Умножение матриц
    constexpr Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for(size_t i = 0; i < 4; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                result.data[i][j] = 0.0f;
                for(size_t k = 0; k < 4; ++k) {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }
    
    // Умножение на вектор
    constexpr Vector4 operator*(const Vector4& v) const {
        return Vector4{
            data[0][0]*v.data[0] + data[0][1]*v.data[1] + data[0][2]*v.data[2] + data[0][3]*v.data[3],
            data[1][0]*v.data[0] + data[1][1]*v.data[1] + data[1][2]*v.data[2] + data[1][3]*v.data[3],
            data[2][0]*v.data[0] + data[2][1]*v.data[1] + data[2][2]*v.data[2] + data[2][3]*v.data[3],
            data[3][0]*v.data[0] + data[3][1]*v.data[1] + data[3][2]*v.data[2] + data[3][3]*v.data[3]
        };
    }
    
    // Умножение на Vector3 (вектор положения, w=1)
    constexpr Vector3 operator*(const Vector3& v) const {
        Vector4 result = *this * Vector4{v, 1.0f};
        return Vector3{result.data[0], result.data[1], result.data[2]};
    }
    
    // Умножение на Vector3 (вектор направления, w=0)
    constexpr Vector3 transform_direction(const Vector3& v) const {
        Vector4 result = *this * Vector4{v, 0.0f};
        return Vector3{result.data[0], result.data[1], result.data[2]};
    }
    
    // Транспонирование
    constexpr Matrix4x4 transposed() const {
        return Matrix4x4{
            data[0][0], data[1][0], data[2][0], data[3][0],
            data[0][1], data[1][1], data[2][1], data[3][1],
            data[0][2], data[1][2], data[2][2], data[3][2],
            data[0][3], data[1][3], data[2][3], data[3][3]
        };
    }
    
    // Транспонирование матрицы
    constexpr void transpose() {
        *this = transposed();
    }
    
    // Определитель
    constexpr real determinant() const {
        // Используем разложение по первой строке
        Matrix3x3 sub0{
            data[1][1], data[1][2], data[1][3],
            data[2][1], data[2][2], data[2][3],
            data[3][1], data[3][2], data[3][3]
        };
        
        Matrix3x3 sub1{
            data[1][0], data[1][2], data[1][3],
            data[2][0], data[2][2], data[2][3],
            data[3][0], data[3][2], data[3][3]
        };
        
        Matrix3x3 sub2{
            data[1][0], data[1][1], data[1][3],
            data[2][0], data[2][1], data[2][3],
            data[3][0], data[3][1], data[3][3]
        };
        
        Matrix3x3 sub3{
            data[1][0], data[1][1], data[1][2],
            data[2][0], data[2][1], data[2][2],
            data[3][0], data[3][1], data[3][2]
        };
        
        return data[0][0] * sub0.determinant() 
             - data[0][1] * sub1.determinant() 
             + data[0][2] * sub2.determinant() 
             - data[0][3] * sub3.determinant();
    }
    
    // Инверсия
    constexpr Matrix4x4 inverted() const {
        real det = determinant();
        if(det == 0.0f) return Matrix4x4{}; // Возвращаем единичную матрицу при нулевом детерминанте
        
        real inv_det = 1.0f / det;
        
        // Вычисляем миноры и сопряженную матрицу
        Matrix4x4 result;
        
        // Для каждой позиции (i,j) вычисляем соответствующий минор
        // Минор - это определитель 3x3 матрицы, полученной удалением i-й строки и j-го столбца
        
        // Позиция (0,0)
        Matrix3x3 minor00{
            data[1][1], data[1][2], data[1][3],
            data[2][1], data[2][2], data[2][3],
            data[3][1], data[3][2], data[3][3]
        };
        result.data[0][0] = minor00.determinant();
        
        // Позиция (0,1)
        Matrix3x3 minor01{
            data[1][0], data[1][2], data[1][3],
            data[2][0], data[2][2], data[2][3],
            data[3][0], data[3][2], data[3][3]
        };
        result.data[0][1] = -minor01.determinant();
        
        // Позиция (0,2)
        Matrix3x3 minor02{
            data[1][0], data[1][1], data[1][3],
            data[2][0], data[2][1], data[2][3],
            data[3][0], data[3][1], data[3][3]
        };
        result.data[0][2] = minor02.determinant();
        
        // Позиция (0,3)
        Matrix3x3 minor03{
            data[1][0], data[1][1], data[1][2],
            data[2][0], data[2][1], data[2][2],
            data[3][0], data[3][1], data[3][2]
        };
        result.data[0][3] = -minor03.determinant();
        
        // Позиция (1,0)
        Matrix3x3 minor10{
            data[0][1], data[0][2], data[0][3],
            data[2][1], data[2][2], data[2][3],
            data[3][1], data[3][2], data[3][3]
        };
        result.data[1][0] = -minor10.determinant();
        
        // Позиция (1,1)
        Matrix3x3 minor11{
            data[0][0], data[0][2], data[0][3],
            data[2][0], data[2][2], data[2][3],
            data[3][0], data[3][2], data[3][3]
        };
        result.data[1][1] = minor11.determinant();
        
        // Позиция (1,2)
        Matrix3x3 minor12{
            data[0][0], data[0][1], data[0][3],
            data[2][0], data[2][1], data[2][3],
            data[3][0], data[3][1], data[3][3]
        };
        result.data[1][2] = -minor12.determinant();
        
        // Позиция (1,3)
        Matrix3x3 minor13{
            data[0][0], data[0][1], data[0][2],
            data[2][0], data[2][1], data[2][2],
            data[3][0], data[3][1], data[3][2]
        };
        result.data[1][3] = minor13.determinant();
        
        // Позиция (2,0)
        Matrix3x3 minor20{
            data[0][1], data[0][2], data[0][3],
            data[1][1], data[1][2], data[1][3],
            data[3][1], data[3][2], data[3][3]
        };
        result.data[2][0] = minor20.determinant();
        
        // Позиция (2,1)
        Matrix3x3 minor21{
            data[0][0], data[0][2], data[0][3],
            data[1][0], data[1][2], data[1][3],
            data[3][0], data[3][2], data[3][3]
        };
        result.data[2][1] = -minor21.determinant();
        
        // Позиция (2,2)
        Matrix3x3 minor22{
            data[0][0], data[0][1], data[0][3],
            data[1][0], data[1][1], data[1][3],
            data[3][0], data[3][1], data[3][3]
        };
        result.data[2][2] = minor22.determinant();
        
        // Позиция (2,3)
        Matrix3x3 minor23{
            data[0][0], data[0][1], data[0][2],
            data[1][0], data[1][1], data[1][2],
            data[3][0], data[3][1], data[3][2]
        };
        result.data[2][3] = -minor23.determinant();
        
        // Позиция (3,0)
        Matrix3x3 minor30{
            data[0][1], data[0][2], data[0][3],
            data[1][1], data[1][2], data[1][3],
            data[2][1], data[2][2], data[2][3]
        };
        result.data[3][0] = -minor30.determinant();
        
        // Позиция (3,1)
        Matrix3x3 minor31{
            data[0][0], data[0][2], data[0][3],
            data[1][0], data[1][2], data[1][3],
            data[2][0], data[2][2], data[2][3]
        };
        result.data[3][1] = minor31.determinant();
        
        // Позиция (3,2)
        Matrix3x3 minor32{
            data[0][0], data[0][1], data[0][3],
            data[1][0], data[1][1], data[1][3],
            data[2][0], data[2][1], data[2][3]
        };
        result.data[3][2] = -minor32.determinant();
        
        // Позиция (3,3)
        Matrix3x3 minor33{
            data[0][0], data[0][1], data[0][2],
            data[1][0], data[1][1], data[1][2],
            data[2][0], data[2][1], data[2][2]
        };
        result.data[3][3] = minor33.determinant();
        
        // Умножаем на обратный детерминант
        for(size_t i = 0; i < 4; ++i) {
            for(size_t j = 0; j < 4; ++j) {
                result.data[i][j] *= inv_det;
            }
        }
        
        return result;
    }
    
    // Инвертирование матрицы
    constexpr void invert() {
        *this = inverted();
    }
    
    // Создание матрицы поворота вокруг оси X
    constexpr static Matrix4x4 rotation_x(real angle) {
        real cos_a = std::cos(angle);
        real sin_a = std::sin(angle);
        
        return Matrix4x4{
            1.0f,  0.0f,  0.0f,  0.0f,
            0.0f,  cos_a, -sin_a, 0.0f,
            0.0f,  sin_a,  cos_a, 0.0f,
            0.0f,  0.0f,  0.0f,  1.0f
        };
    }
    
    // Создание матрицы поворота вокруг оси Y
    constexpr static Matrix4x4 rotation_y(real angle) {
        real cos_a = std::cos(angle);
        real sin_a = std::sin(angle);
        
        return Matrix4x4{
            cos_a, 0.0f,  sin_a, 0.0f,
            0.0f,  1.0f,  0.0f,  0.0f,
            -sin_a, 0.0f, cos_a, 0.0f,
            0.0f,  0.0f,  0.0f,  1.0f
        };
    }
    
    // Создание матрицы поворота вокруг оси Z
    constexpr static Matrix4x4 rotation_z(real angle) {
        real cos_a = std::cos(angle);
        real sin_a = std::sin(angle);
        
        return Matrix4x4{
            cos_a, -sin_a, 0.0f, 0.0f,
            sin_a,  cos_a, 0.0f, 0.0f,
            0.0f,   0.0f,  1.0f, 0.0f,
            0.0f,   0.0f,  0.0f,  1.0f
        };
    }
    
    // Создание матрицы поворота из кватерниона
    constexpr static Matrix4x4 from_quaternion(const Quaternion& q);
    
    // Создание масштабирующей матрицы
    constexpr static Matrix4x4 scale(real sx, real sy, real sz) {
        return Matrix4x4{
            sx,   0.0f, 0.0f, 0.0f,
            0.0f, sy,   0.0f, 0.0f,
            0.0f, 0.0f, sz,   0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    // Создание масштабирующей матрицы (одинаковый масштаб по всем осям)
    constexpr static Matrix4x4 scale(real s) {
        return scale(s, s, s);
    }
    
    // Создание трансляционной матрицы
    constexpr static Matrix4x4 translation(real tx, real ty, real tz) {
        return Matrix4x4{
            1.0f, 0.0f, 0.0f, tx,
            0.0f, 1.0f, 0.0f, ty,
            0.0f, 0.0f, 1.0f, tz,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    // Создание трансляционной матрицы из вектора
    constexpr static Matrix4x4 translation(const Vector3& t) {
        return translation(t.x(), t.y(), t.z());
    }
    
    // Создание перспективной матрицы (левосторонняя система координат)
    constexpr static Matrix4x4 perspective_lh(real fov, real aspect, real near_plane, real far_plane) {
        real tan_half_fov = std::tan(fov * 0.5f);
        
        return Matrix4x4{
            1.0f / (aspect * tan_half_fov), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
            0.0f, 0.0f, far_plane / (far_plane - near_plane), -near_plane * far_plane / (far_plane - near_plane),
            0.0f, 0.0f, 1.0f, 0.0f
        };
    }
    
    // Создание перспективной матрицы (правосторонняя система координат)
    constexpr static Matrix4x4 perspective_rh(real fov, real aspect, real near_plane, real far_plane) {
        real tan_half_fov = std::tan(fov * 0.5f);
        
        return Matrix4x4{
            1.0f / (aspect * tan_half_fov), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
            0.0f, 0.0f, far_plane / (near_plane - far_plane), near_plane * far_plane / (near_plane - far_plane),
            0.0f, 0.0f, -1.0f, 0.0f
        };
    }
    
    // Создание ортографической матрицы (левосторонняя система координат)
    constexpr static Matrix4x4 orthographic_lh(real left, real right, real bottom, real top, real near_plane, real far_plane) {
        return Matrix4x4{
            2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
            0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
            0.0f, 0.0f, 1.0f / (far_plane - near_plane), -near_plane / (far_plane - near_plane),
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    // Создание ортографической матрицы (правосторонняя система координат)
    constexpr static Matrix4x4 orthographic_rh(real left, real right, real bottom, real top, real near_plane, real far_plane) {
        return Matrix4x4{
            2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
            0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
            0.0f, 0.0f, -1.0f / (far_plane - near_plane), -near_plane / (far_plane - near_plane),
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    // Создание матрицы вида (левосторонняя система координат)
    constexpr static Matrix4x4 look_at_lh(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 forward = (target - eye).normalized();
        Vector3 right = forward.cross(up).normalized();
        Vector3 up_new = right.cross(forward);
        
        return Matrix4x4{
            right.x(), up_new.x(), forward.x(), -right.dot(eye),
            right.y(), up_new.y(), forward.y(), -up_new.dot(eye),
            right.z(), up_new.z(), forward.z(), -forward.dot(eye),
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    // Создание матрицы вида (правосторонняя система координат)
    constexpr static Matrix4x4 look_at_rh(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 forward = (eye - target).normalized();
        Vector3 right = up.cross(forward).normalized();
        Vector3 up_new = forward.cross(right);
        
        return Matrix4x4{
            right.x(), right.y(), right.z(), -right.dot(eye),
            up_new.x(), up_new.y(), up_new.z(), -up_new.dot(eye),
            forward.x(), forward.y(), forward.z(), -forward.dot(eye),
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
};

// Класс кватерниона
struct Quaternion : public VectorBase<4> {
    // Конструкторы
    constexpr Quaternion() : VectorBase<4>{{0.0f, 0.0f, 0.0f, 1.0f}} {} // Идентичный кватернион
    constexpr Quaternion(real x, real y, real z, real w) : VectorBase<4>{{x, y, z, w}} {}
    constexpr explicit Quaternion(const Vector3& axis, real angle) : VectorBase<4>() {
        real half_angle = angle * 0.5f;
        real sin_half = std::sin(half_angle);
        real cos_half = std::cos(half_angle);
        
        Vector3 normalized_axis = axis.normalized();
        data[0] = normalized_axis.x() * sin_half;
        data[1] = normalized_axis.y() * sin_half;
        data[2] = normalized_axis.z() * sin_half;
        data[3] = cos_half;
    }
    
    // Получение компонентов
    constexpr real x() const { return data[0]; }
    constexpr real y() const { return data[1]; }
    constexpr real z() const { return data[2]; }
    constexpr real w() const { return data[3]; }
    
    // Создание кватерниона поворота из углов Эйлера (XYZ)
    constexpr static Quaternion from_euler_angles(real pitch, real yaw, real roll) {
        // Преобразуем углы в половинные
        real cp = std::cos(pitch * 0.5f);
        real sp = std::sin(pitch * 0.5f);
        real cy = std::cos(yaw * 0.5f);
        real sy = std::sin(yaw * 0.5f);
        real cr = std::cos(roll * 0.5f);
        real sr = std::sin(roll * 0.5f);
        
        return Quaternion{
            cr*sp*cy - sr*cp*sy,  // x
            cr*cp*sy + sr*sp*cy,  // y
            sr*cp*cy - cr*sp*sy,  // z
            cr*cp*cy + sr*sp*sy   // w
        };
    }
    
    // Операторы присваивания
    constexpr Quaternion& operator+=(const Quaternion& other) {
        data[0] += other.data[0];
        data[1] += other.data[1];
        data[2] += other.data[2];
        data[3] += other.data[3];
        return *this;
    }
    
    constexpr Quaternion& operator-=(const Quaternion& other) {
        data[0] -= other.data[0];
        data[1] -= other.data[1];
        data[2] -= other.data[2];
        data[3] -= other.data[3];
        return *this;
    }
    
    // Умножение кватернионов
    constexpr Quaternion& operator*=(const Quaternion& other) {
        real new_x = data[3]*other.data[0] + data[0]*other.data[3] + data[1]*other.data[2] - data[2]*other.data[1];
        real new_y = data[3]*other.data[1] - data[0]*other.data[2] + data[1]*other.data[3] + data[2]*other.data[0];
        real new_z = data[3]*other.data[2] + data[0]*other.data[1] - data[1]*other.data[0] + data[2]*other.data[3];
        real new_w = data[3]*other.data[3] - data[0]*other.data[0] - data[1]*other.data[1] - data[2]*other.data[2];
        
        data[0] = new_x;
        data[1] = new_y;
        data[2] = new_z;
        data[3] = new_w;
        
        return *this;
    }
    
    // Умножение на скаляр
    constexpr Quaternion& operator*=(real scalar) {
        data[0] *= scalar;
        data[1] *= scalar;
        data[2] *= scalar;
        data[3] *= scalar;
        return *this;
    }
    
    // Длина кватерниона
    constexpr real length() const {
        return std::sqrt(length_squared());
    }
    
    // Квадрат длины кватерниона
    constexpr real length_squared() const {
        return data[0]*data[0] + data[1]*data[1] + data[2]*data[2] + data[3]*data[3];
    }
    
    // Нормализация кватерниона
    constexpr Quaternion normalized() const {
        real len = length();
        if(len > 0.0f) {
            return Quaternion{data[0]/len, data[1]/len, data[2]/len, data[3]/len};
        }
        return Quaternion{0.0f, 0.0f, 0.0f, 1.0f};
    }
    
    // Нормализация кватерниона
    constexpr void normalize() {
        *this = normalized();
    }
    
    // Сопряжение кватерниона
    constexpr Quaternion conjugated() const {
        return Quaternion{-data[0], -data[1], -data[2], data[3]};
    }
    
    // Инверсия кватерниона
    constexpr Quaternion inverted() const {
        return conjugated() * (1.0f / length_squared());
    }
    
    // Поворот вектора с помощью кватерниона
    constexpr Vector3 rotate(const Vector3& v) const {
        // q * (v, 0) * q_conjugate
        Quaternion v_quat{v.x(), v.y(), v.z(), 0.0f};
        Quaternion result = *this * v_quat * conjugated();
        return Vector3{result.x(), result.y(), result.z()};
    }
    
    // Линейная интерполяция (LERP)
    constexpr Quaternion lerp(const Quaternion& other, real t) const {
        return Quaternion{
            data[0] + (other.data[0] - data[0]) * t,
            data[1] + (other.data[1] - data[1]) * t,
            data[2] + (other.data[2] - data[2]) * t,
            data[3] + (other.data[3] - data[3]) * t
        }.normalized();
    }
    
    // Сферическая линейная интерполяция (SLERP)
    constexpr Quaternion slerp(const Quaternion& other, real t) const {
        real cos_half_theta = dot(other);
        
        if(cos_half_theta >= 1.0f) {
            // Кватернионы уже равны
            return *this;
        }
        
        // Если скалярное произведение отрицательное, инвертируем один из кватернионов
        // для получения кратчайшего пути
        Quaternion other_q = other;
        if(cos_half_theta < 0.0f) {
            cos_half_theta = -cos_half_theta;
            other_q = Quaternion{-other.data[0], -other.data[1], -other.data[2], -other.data[3]};
        }
        
        if(cos_half_theta > 0.9999f) {
            // Если кватернионы очень близки, используем LERP
            return lerp(other_q, t);
        }
        
        real half_theta = std::acos(cos_half_theta);
        real sin_half_theta = std::sin(half_theta);
        
        real a = std::sin((1.0f - t) * half_theta) / sin_half_theta;
        real b = std::sin(t * half_theta) / sin_half_theta;
        
        return Quaternion{
            data[0]*a + other_q.data[0]*b,
            data[1]*a + other_q.data[1]*b,
            data[2]*a + other_q.data[2]*b,
            data[3]*a + other_q.data[3]*b
        };
    }
    
    // Скалярное произведение кватернионов
    constexpr real dot(const Quaternion& other) const {
        return data[0]*other.data[0] + data[1]*other.data[1] + data[2]*other.data[2] + data[3]*other.data[3];
    }
};

// Операторы для кватернионов
constexpr Quaternion operator+(const Quaternion& a, const Quaternion& b) {
    return Quaternion{a.data[0] + b.data[0], a.data[1] + b.data[1], a.data[2] + b.data[2], a.data[3] + b.data[3]};
}

constexpr Quaternion operator-(const Quaternion& a, const Quaternion& b) {
    return Quaternion{a.data[0] - b.data[0], a.data[1] - b.data[1], a.data[2] - b.data[2], a.data[3] - b.data[3]};
}

constexpr Quaternion operator*(const Quaternion& a, const Quaternion& b) {
    return Quaternion{
        a.data[3]*b.data[0] + a.data[0]*b.data[3] + a.data[1]*b.data[2] - a.data[2]*b.data[1],
        a.data[3]*b.data[1] - a.data[0]*b.data[2] + a.data[1]*b.data[3] + a.data[2]*b.data[0],
        a.data[3]*b.data[2] + a.data[0]*b.data[1] - a.data[1]*b.data[0] + a.data[2]*b.data[3],
        a.data[3]*b.data[3] - a.data[0]*b.data[0] - a.data[1]*b.data[1] - a.data[2]*b.data[2]
    };
}

constexpr Quaternion operator*(const Quaternion& q, real scalar) {
    return Quaternion{q.data[0]*scalar, q.data[1]*scalar, q.data[2]*scalar, q.data[3]*scalar};
}

constexpr Quaternion operator*(real scalar, const Quaternion& q) {
    return Quaternion{q.data[0]*scalar, q.data[1]*scalar, q.data[2]*scalar, q.data[3]*scalar};
}

// Реализация метода из кватерниона в матрицу 4x4
constexpr Matrix4x4 Matrix4x4::from_quaternion(const Quaternion& q) {
    real xx = q.x() * q.x();
    real xy = q.x() * q.y();
    real xz = q.x() * q.z();
    real xw = q.x() * q.w();
    
    real yy = q.y() * q.y();
    real yz = q.y() * q.z();
    real yw = q.y() * q.w();
    
    real zz = q.z() * q.z();
    real zw = q.z() * q.w();
    
    return Matrix4x4{
        1.0f - 2.0f * (yy + zz), 2.0f * (xy - zw), 2.0f * (xz + yw), 0.0f,
        2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - xw), 0.0f,
        2.0f * (xz - yw), 2.0f * (yz + xw), 1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

// Системы координат
enum class Handedness {
    Left,
    Right
};

// Функции для работы с разными системами координат
template<Handedness H>
struct CoordSystem {
    // Матрица вида в зависимости от системы координат
    constexpr static Matrix4x4 look_at(const Vector3& eye, const Vector3& target, const Vector3& up) {
        if constexpr (H == Handedness::Left) {
            return Matrix4x4::look_at_lh(eye, target, up);
        } else {
            return Matrix4x4::look_at_rh(eye, target, up);
        }
    }
    
    // Перспективная матрица в зависимости от системы координат
    constexpr static Matrix4x4 perspective(real fov, real aspect, real near_plane, real far_plane) {
        if constexpr (H == Handedness::Left) {
            return Matrix4x4::perspective_lh(fov, aspect, near_plane, far_plane);
        } else {
            return Matrix4x4::perspective_rh(fov, aspect, near_plane, far_plane);
        }
    }
    
    // Ортографическая матрица в зависимости от системы координат
    constexpr static Matrix4x4 orthographic(real left, real right, real bottom, real top, real near_plane, real far_plane) {
        if constexpr (H == Handedness::Left) {
            return Matrix4x4::orthographic_lh(left, right, bottom, top, near_plane, far_plane);
        } else {
            return Matrix4x4::orthographic_rh(left, right, bottom, top, near_plane, far_plane);
        }
    }
};

// Псевдонимы для удобства
using LeftHanded = CoordSystem<Handedness::Left>;
using RightHanded = CoordSystem<Handedness::Right>;

// Класс трансформации для 3D-объектов
class Transform {
private:
    Vector3 m_position;
    Vector3 m_rotation; // в градусах
    Vector3 m_scale;
    Transform* m_parent;
    mutable Matrix4x4 m_local_matrix;
    mutable Matrix4x4 m_world_matrix;
    mutable bool m_local_matrix_dirty;
    mutable bool m_world_matrix_dirty;

    // Обновление локальной матрицы при изменении параметров
    void mark_dirty() const {
        m_local_matrix_dirty = true;
        m_world_matrix_dirty = true;
    }

public:
    // Конструктор по умолчанию
    constexpr Transform()
        : m_position(0.0f, 0.0f, 0.0f)
        , m_rotation(0.0f, 0.0f, 0.0f)
        , m_scale(1.0f, 1.0f, 1.0f)
        , m_parent(nullptr)
        , m_local_matrix_dirty(true)
        , m_world_matrix_dirty(true) {}

    // Конструктор с параметрами
    constexpr Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale)
        : m_position(position)
        , m_rotation(rotation)
        , m_scale(scale)
        , m_parent(nullptr)
        , m_local_matrix_dirty(true)
        , m_world_matrix_dirty(true) {}

    // Получение и установка позиции
    constexpr const Vector3& get_position() const { return m_position; }
    constexpr void set_position(const Vector3& pos) {
        m_position = pos;
        mark_dirty();
    }
    constexpr void set_position(real x, real y, real z) {
        m_position = Vector3(x, y, z);
        mark_dirty();
    }

    // Получение и установка вращения
    constexpr const Vector3& get_rotation() const { return m_rotation; }
    constexpr void set_rotation(const Vector3& rot) {
        m_rotation = rot;
        mark_dirty();
    }
    constexpr void set_rotation(real x, real y, real z) {
        m_rotation = Vector3(x, y, z);
        mark_dirty();
    }

    // Получение и установка масштаба
    constexpr const Vector3& get_scale() const { return m_scale; }
    constexpr void set_scale(const Vector3& scale) {
        m_scale = scale;
        mark_dirty();
    }
    constexpr void set_scale(real x, real y, real z) {
        m_scale = Vector3(x, y, z);
        mark_dirty();
    }

    // Получение и установка родителя
    constexpr Transform* get_parent() const { return m_parent; }
    constexpr void set_parent(Transform* parent) {
        m_parent = parent;
        m_world_matrix_dirty = true;
    }

    // Получение глобальной позиции с учетом иерархии
    constexpr Vector3 get_global_position() const {
        if (m_parent) {
            Matrix4x4 parent_matrix = m_parent->get_world_matrix();
            Vector4 pos_h = parent_matrix * Vector4(m_position, 1.0f);
            return Vector3(pos_h.x(), pos_h.y(), pos_h.z());
        }
        return m_position;
    }

    // Получение глобального масштаба с учетом иерархии
    constexpr Vector3 get_global_scale() const {
        if (m_parent) {
            Vector3 parent_scale = m_parent->get_global_scale();
            return Vector3(m_scale.x() * parent_scale.x(),
                          m_scale.y() * parent_scale.y(),
                          m_scale.z() * parent_scale.z());
        }
        return m_scale;
    }

    // Получение глобального вращения с учетом иерархии
    constexpr Vector3 get_global_rotation() const {
        if (m_parent) {
            Vector3 parent_rot = m_parent->get_global_rotation();
            return Vector3(m_rotation.x() + parent_rot.x(),
                          m_rotation.y() + parent_rot.y(),
                          m_rotation.z() + parent_rot.z());
        }
        return m_rotation;
    }

    // Перемещение относительно локальных осей
    constexpr void translate(const Vector3& offset) {
        m_position = m_position + offset;
        mark_dirty();
    }
    constexpr void translate(real x, real y, real z) {
        m_position = m_position + Vector3(x, y, z);
        mark_dirty();
    }

    // Поворот относительно локальных осей
    constexpr void rotate(const Vector3& rotation) {
        m_rotation = m_rotation + rotation;
        mark_dirty();
    }
    constexpr void rotate(real x, real y, real z) {
        m_rotation = m_rotation + Vector3(x, y, z);
        mark_dirty();
    }

    // Масштабирование
    constexpr void scale(const Vector3& scale_factor) {
        m_scale = Vector3(m_scale.x() * scale_factor.x(),
                         m_scale.y() * scale_factor.y(),
                         m_scale.z() * scale_factor.z());
        mark_dirty();
    }
    constexpr void scale(real x, real y, real z) {
        m_scale = Vector3(m_scale.x() * x,
                         m_scale.y() * y,
                         m_scale.z() * z);
        mark_dirty();
    }

    // Вычисление локальной матрицы трансформации
    constexpr const Matrix4x4& get_local_matrix() const {
        if (m_local_matrix_dirty) {
            // Преобразуем градусы в радианы
            real rx = m_rotation.x() * math_constants::DEG_TO_RAD;
            real ry = m_rotation.y() * math_constants::DEG_TO_RAD;
            real rz = m_rotation.z() * math_constants::DEG_TO_RAD;

            // Создаем матрицы трансформации
            Matrix4x4 translation = Matrix4x4::translation(m_position);
            Matrix4x4 rotation_x = Matrix4x4::rotation_x(rx);
            Matrix4x4 rotation_y = Matrix4x4::rotation_y(ry);
            Matrix4x4 rotation_z = Matrix4x4::rotation_z(rz);
            Matrix4x4 scale_matrix = Matrix4x4::scale(m_scale.x(), m_scale.y(), m_scale.z());

           // Комбинируем: сначала масштаб, затем вращение (Z, Y, X), затем перенос
           // Правильный порядок: T * R * S
           m_local_matrix = translation * rotation_z * rotation_y * rotation_x * scale_matrix;
           m_local_matrix_dirty = false;
       }
       return m_local_matrix;
   }

    // Вычисление мировой матрицы трансформации
    constexpr const Matrix4x4& get_world_matrix() const {
        if (m_world_matrix_dirty) {
            if (m_parent) {
                m_world_matrix = m_parent->get_world_matrix() * get_local_matrix();
            } else {
                m_world_matrix = get_local_matrix();
            }
            m_world_matrix_dirty = false;
        }
        return m_world_matrix;
    }

    // Получение обратной мировой матрицы (для преобразования из мировых в локальные координаты)
    constexpr Matrix4x4 get_inverse_world_matrix() const {
        Matrix4x4 world = get_world_matrix();
        return world.inverted();
    }

    // Преобразование точки из локальных в мировые координаты
    constexpr Vector3 local_to_world(const Vector3& point) const {
        Vector4 result = get_world_matrix() * Vector4(point, 1.0f);
        return Vector3(result.x(), result.y(), result.z());
    }

    // Преобразование точки из мировых в локальные координаты
    constexpr Vector3 world_to_local(const Vector3& point) const {
        Vector4 result = get_inverse_world_matrix() * Vector4(point, 1.0f);
        return Vector3(result.x(), result.y(), result.z());
    }

    // Метод LookAt для ориентации трансформации по направлению
    constexpr void look_at(const Vector3& target, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f)) {
        Vector3 forward = (target - get_global_position()).normalized();
        
        // Вычисляем правый вектор
        Vector3 right = forward.cross(up).normalized();
        // Пересчитываем верхний вектор для ортогональности
        Vector3 new_up = right.cross(forward);
        
        // Преобразуем вращение из матрицы в углы Эйлера (упрощенный метод)
        // Вычисляем углы поворота
        real pitch = std::asin(-new_up.y());
        real yaw = std::atan2(right.y(), new_up.y());
        real roll = std::atan2(forward.x(), forward.y());
        
        // Преобразуем из радиан в градусы
        m_rotation = Vector3(pitch * math_constants::RAD_TO_DEG,
                            yaw * math_constants::RAD_TO_DEG,
                            roll * math_constants::RAD_TO_DEG);
        mark_dirty();
    }

    // Сброс трансформации к единичной
    constexpr void reset() {
        m_position = Vector3(0.0f, 0.0f, 0.0f);
        m_rotation = Vector3(0.0f, 0.0f, 0.0f);
        m_scale = Vector3(1.0f, 1.0f, 1.0f);
        mark_dirty();
    }
};

// Класс плоскости
struct Plane {
    Vector3 normal;  // Нормаль к плоскости (должна быть нормализована)
    real distance;   // Расстояние от начала координат до плоскости вдоль нормали

    // Конструктор
    constexpr Plane() : normal(0.0f, 0.0f, 1.0f), distance(0.0f) {}
    constexpr Plane(const Vector3& n, real d) : normal(n.normalized()), distance(d) {}
    constexpr Plane(const Vector3& n, const Vector3& point) : normal(n.normalized()), distance(n.normalized().dot(point)) {}
    
    // Создание плоскости по трем точкам
    constexpr static Plane from_points(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
        Vector3 edge1 = p2 - p1;
        Vector3 edge2 = p3 - p1;
        Vector3 n = edge1.cross(edge2).normalized();
        return Plane(n, n.dot(p1));
    }
    
    // Расстояние от точки до плоскости
    constexpr real distance_to_point(const Vector3& point) const {
        return normal.dot(point) - distance;
    }
    
    // Проверка, находится ли точка над плоскостью
    constexpr bool is_point_over_plane(const Vector3& point) const {
        return distance_to_point(point) > 0.0f;
    }
    
    // Проверка, находится ли точка под плоскостью
    constexpr bool is_point_under_plane(const Vector3& point) const {
        return distance_to_point(point) < 0.0f;
    }
    
    // Проверка, находится ли точка на плоскости
    constexpr bool is_point_on_plane(const Vector3& point, real epsilon = 1e-5f) const {
        return std::abs(distance_to_point(point)) < epsilon;
    }
    
    // Пересечение луча с плоскостью
    constexpr bool ray_intersect(const Vector3& ray_origin, const Vector3& ray_direction, real& t) const {
        real denominator = normal.dot(ray_direction);
        if (std::abs(denominator) < 1e-6f) {
            // Луч параллелен плоскости
            return false;
        }
        
        t = (distance - normal.dot(ray_origin)) / denominator;
        return t >= 0.0f;  // Пересечение только впереди по направлению луча
    }
    
    // Отражение точки относительно плоскости
    constexpr Vector3 reflect_point(const Vector3& point) const {
        real dist = distance_to_point(point);
        return point - normal * (2.0f * dist);
    }
};

// Класс сферы
struct Sphere {
    Vector3 center;
    real radius;

    // Конструктор
    constexpr Sphere() : center(0.0f, 0.0f, 0.0f), radius(1.0f) {}
    constexpr Sphere(const Vector3& c, real r) : center(c), radius(r > 0.0f ? r : 0.0f) {}
    
    // Проверка, находится ли точка внутри сферы
    constexpr bool contains_point(const Vector3& point) const {
        return (point - center).length_squared() <= radius * radius;
    }
    
    // Проверка пересечения с другой сферой
    constexpr bool intersects_sphere(const Sphere& other) const {
        real distance_sq = (center - other.center).length_squared();
        real radius_sum = radius + other.radius;
        return distance_sq <= radius_sum * radius_sum;
    }
    
    // Объем сферы
    constexpr real volume() const {
        return (4.0f / 3.0f) * math_constants::PI * radius * radius * radius;
    }
    
    // Площадь поверхности сферы
    constexpr real surface_area() const {
        return 4.0f * math_constants::PI * radius * radius;
    }
    
    // Пересечение луча со сферой
    constexpr bool ray_intersect(const Vector3& ray_origin, const Vector3& ray_direction, real& t1, real& t2) const {
        Vector3 oc = ray_origin - center;
        real a = ray_direction.dot(ray_direction);
        real b = 2.0f * oc.dot(ray_direction);
        real c = oc.dot(oc) - radius * radius;
        real discriminant = b * b - 4 * a * c;
        
        if (discriminant < 0) {
            return false;  // Нет пересечения
        } else if (discriminant == 0) {
            // Касательная
            t1 = t2 = -b / (2.0f * a);
            return t1 >= 0.0f;
        } else {
            // Два пересечения
            real sqrt_discriminant = std::sqrt(discriminant);
            t1 = (-b - sqrt_discriminant) / (2.0f * a);
            t2 = (-b + sqrt_discriminant) / (2.0f * a);
            
            // Возвращаем только пересечения впереди по направлению луча
            if (t1 < 0 && t2 < 0) return false;
            if (t1 < 0) t1 = t2;  // Если первая точка позади, используем вторую
            if (t2 < 0) t2 = t1;  // Если вторая точка позади, используем первую
            return true;
        }
    }
    
    // Пересечение луча со сферой (только ближайшее пересечение)
    constexpr bool ray_intersect(const Vector3& ray_origin, const Vector3& ray_direction, real& t) const {
        real t1, t2;
        if (ray_intersect(ray_origin, ray_direction, t1, t2)) {
            t = (t1 >= 0) ? t1 : t2;
            return t >= 0;
        }
        return false;
    }
    
    // Ближайшая точка на сфере к заданной точке
    constexpr Vector3 closest_point_on_surface(const Vector3& point) const {
        Vector3 direction = (point - center).normalized();
        return center + direction * radius;
    }
};

// Класс осевы-aligned ограничивающего объема (AABB)
struct AABB {
    Vector3 min;
    Vector3 max;

    // Конструктор
    constexpr AABB() : min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f) {}
    constexpr AABB(const Vector3& min_point, const Vector3& max_point) : min(min_point), max(max_point) {
        // Убедимся, что min <= max
        if (min.x() > max.x()) std::swap(min[0], max[0]);
        if (min.y() > max.y()) std::swap(min[1], max[1]);
        if (min.z() > max.z()) std::swap(min[2], max[2]);
    }
    
    // Создание AABB из центра и размеров
    constexpr static AABB from_center_extents(const Vector3& center, const Vector3& extents) {
        return AABB(center - extents, center + extents);
    }
    
    // Создание AABB из сферы
    constexpr static AABB from_sphere(const Sphere& sphere) {
        Vector3 extents(sphere.radius, sphere.radius, sphere.radius);
        return AABB(sphere.center - extents, sphere.center + extents);
    }
    
    // Проверка, содержит ли AABB точку
    constexpr bool contains_point(const Vector3& point) const {
        return point.x() >= min.x() && point.x() <= max.x() &&
               point.y() >= min.y() && point.y() <= max.y() &&
               point.z() >= min.z() && point.z() <= max.z();
    }
    
    // Проверка пересечения с другим AABB
    constexpr bool intersects_aabb(const AABB& other) const {
        return !(min.x() > other.max.x() || other.min.x() > max.x() ||
                 min.y() > other.max.y() || other.min.y() > max.y() ||
                 min.z() > other.max.z() || other.min.z() > max.z());
    }
    
    // Объем AABB
    constexpr real volume() const {
        Vector3 size = max - min;
        return size.x() * size.y() * size.z();
    }
    
    // Площадь поверхности AABB
    constexpr real surface_area() const {
        Vector3 size = max - min;
        return 2.0f * (size.x() * size.y() + size.x() * size.z() + size.y() * size.z());
    }
    
    // Центр AABB
    constexpr Vector3 center() const {
        return (min + max) * 0.5f;
    }
    
    // Размеры AABB
    constexpr Vector3 size() const {
        return max - min;
    }
    
    // Половинные размеры AABB
    constexpr Vector3 half_extents() const {
        return (max - min) * 0.5f;
    }
    
    // Объединение двух AABB
    constexpr AABB merged_with(const AABB& other) const {
        Vector3 new_min(
            std::min(min.x(), other.min.x()),
            std::min(min.y(), other.min.y()),
            std::min(min.z(), other.min.z())
        );
        Vector3 new_max(
            std::max(max.x(), other.max.x()),
            std::max(max.y(), other.max.y()),
            std::max(max.z(), other.max.z())
        );
        return AABB(new_min, new_max);
    }
    
    // Расширение AABB для включения точки
    constexpr AABB expanded_to_include(const Vector3& point) const {
        Vector3 new_min(
            std::min(min.x(), point.x()),
            std::min(min.y(), point.y()),
            std::min(min.z(), point.z())
        );
        Vector3 new_max(
            std::max(max.x(), point.x()),
            std::max(max.y(), point.y()),
            std::max(max.z(), point.z())
        );
        return AABB(new_min, new_max);
    }
};

// Класс ориентированного ограничивающего объема (OBB)
struct OBB {
    Vector3 center;
    Vector3 half_extents;  // Половинные размеры вдоль локальных осей
    Matrix3x3 orientation; // Матрица ориентации (столбцовые векторы - оси OBB)

    // Конструктор
    constexpr OBB() : center(0.0f, 0.0f, 0.0f), half_extents(0.5f, 0.5f, 0.5f), orientation(Matrix3x3{}) {}
    constexpr OBB(const Vector3& c, const Vector3& he, const Matrix3x3& orient)
        : center(c), half_extents(he), orientation(orient) {}
    
    // Создание OBB из AABB
    constexpr static OBB from_aabb(const AABB& aabb) {
        return OBB(
            aabb.center(),
            aabb.half_extents(),
            Matrix3x3{}  // единичная матрица (не вращена)
        );
    }
    
    // Объем OBB
    constexpr real volume() const {
        return 8.0f * half_extents.x() * half_extents.y() * half_extents.z();
    }
    
    // Площадь поверхности OBB
    constexpr real surface_area() const {
        real x = half_extents.x() * 2.0f;
        real y = half_extents.y() * 2.0f;
        real z = half_extents.z() * 2.0f;
        return 2.0f * (x*y + x*z + y*z);
    }
    
    // Получение вершин OBB
    constexpr void get_vertices(Vector3 vertices[8]) const {
        Vector3 he = half_extents;
        // 8 вершин в локальных координатах
        Vector3 local_vertices[8] = {
            Vector3(-he.x(), -he.y(), -he.z()),
            Vector3(he.x(), -he.y(), -he.z()),
            Vector3(he.x(), he.y(), -he.z()),
            Vector3(-he.x(), he.y(), -he.z()),
            Vector3(-he.x(), -he.y(), he.z()),
            Vector3(he.x(), -he.y(), he.z()),
            Vector3(he.x(), he.y(), he.z()),
            Vector3(-he.x(), he.y(), he.z())
        };
        
        // Преобразование в мировые координаты
        for(int i = 0; i < 8; i++) {
            Vector3 local = orientation * local_vertices[i];
            vertices[i] = center + local;
        }
    }
};

// Класс капсулы
struct Capsule {
    Vector3 point_a;  // Первая точка осевого отрезка
    Vector3 point_b;  // Вторая точка осевого отрезка
    real radius;

    // Конструктор
    constexpr Capsule() : point_a(0.0f, -0.5f, 0.0f), point_b(0.0f, 0.5f, 0.0f), radius(0.5f) {}
    constexpr Capsule(const Vector3& a, const Vector3& b, real r) : point_a(a), point_b(b), radius(r > 0.0f ? r : 0.0f) {}
    
    // Длина осевого отрезка
    constexpr real axis_length() const {
        return (point_b - point_a).length();
    }
    
    // Объем капсулы
    constexpr real volume() const {
        real cylinder_volume = math_constants::PI * radius * radius * axis_length();
        real sphere_volume = (4.0f / 3.0f) * math_constants::PI * radius * radius * radius;
        return cylinder_volume + sphere_volume;
    }
    
    // Площадь поверхности капсулы
    constexpr real surface_area() const {
        real cylinder_area = 2.0f * math_constants::PI * radius * axis_length();
        real sphere_area = 4.0f * math_constants::PI * radius * radius;
        return cylinder_area + sphere_area;
    }
    
    // Центр капсулы
    constexpr Vector3 center() const {
        return (point_a + point_b) * 0.5f;
    }
    
    // Ближайшая точка на осевом отрезке к заданной точке
    constexpr Vector3 closest_point_on_axis(const Vector3& point) const {
        Vector3 ab = point_b - point_a;
        real length_sq = ab.length_squared();
        
        if (length_sq < 1e-6f) {
            // Отрезок вырожден в точку
            return point_a;
        }
        
        real t = (point - point_a).dot(ab) / length_sq;
        t = std::clamp(t, 0.0f, 1.0f);  // Ограничиваем параметр [0, 1]
        return point_a + ab * t;
    }
    
    // Ближайшая точка на поверхности капсулы к заданной точке
    constexpr Vector3 closest_point_on_surface(const Vector3& point) const {
        Vector3 closest_on_axis = closest_point_on_axis(point);
        Vector3 direction = (point - closest_on_axis).normalized();
        return closest_on_axis + direction * radius;
    }
    
    // Проверка, находится ли точка внутри капсулы
    constexpr bool contains_point(const Vector3& point) const {
        Vector3 closest_on_axis = closest_point_on_axis(point);
        return (point - closest_on_axis).length_squared() <= radius * radius;
    }
};

// Класс конуса
struct Cone {
    Vector3 base_center; // Центр основания
    Vector3 axis;         // Направление и высота (длина вектора = высота)
    real base_radius;     // Радиус основания

    // Конструктор
    constexpr Cone() : base_center(0.0f, 0.0f, 0.0f), axis(0.0f, 1.0f, 0.0f), base_radius(1.0f) {}
    constexpr Cone(const Vector3& base_center, const Vector3& axis_dir, real height, real base_rad)
        : base_center(base_center), axis(axis_dir.normalized() * height), base_radius(base_rad > 0.0f ? base_rad : 0.0f) {}
    
    // Высота конуса
    constexpr real height() const {
        return axis.length();
    }
    
    // Направление оси
    constexpr Vector3 axis_direction() const {
        return axis.normalized();
    }
    
    // Вершина конуса
    constexpr Vector3 apex() const {
        return base_center + axis;
    }
    
    // Объем конуса
    constexpr real volume() const {
        return (1.0f / 3.0f) * math_constants::PI * base_radius * base_radius * height();
    }
    
    // Площадь поверхности конуса (боковая + основание)
    constexpr real surface_area() const {
        real slant_height = std::sqrt(base_radius * base_radius + height() * height());
        return math_constants::PI * base_radius * (base_radius + slant_height);
    }
    
    // Проверка, находится ли точка внутри конуса
    constexpr bool contains_point(const Vector3& point) const {
        Vector3 to_point = point - base_center;
        Vector3 axis_dir = axis_direction();
        real axis_len = height();
        
        // Проекция точки на ось конуса
        real projection = to_point.dot(axis_dir);
        
        // Если проекция за пределами высоты конуса
        if (projection < 0.0f || projection > axis_len) {
            return false;
        }
        
        // Радиус на уровне проекции
        real local_radius = base_radius * (1.0f - projection / axis_len);
        
        // Перпендикулярное расстояние от оси
        Vector3 projected = axis_dir * projection;
        Vector3 perpendicular = to_point - projected;
        real distance_from_axis = perpendicular.length();
        
        return distance_from_axis <= local_radius;
    }
};

// Класс фрустума (усеченная пирамида видимости)
struct Frustum {
    Plane planes[6];  // 6 плоскостей: левая, правая, верхняя, нижняя, ближняя, дальней
    
    // Конструктор
    constexpr Frustum() = default;
    
    // Создание фрустума из матрицы вида-проекции
    constexpr static Frustum from_matrix(const Matrix4x4& matrix) {
        Frustum frustum;
        
        // Левые и правые плоскости
        frustum.planes[0] = Plane(matrix[3] + matrix[0], matrix[3][0] + matrix[0][0], matrix[3][1] + matrix[0][1], matrix[3][2] + matrix[0][2]); // лево
        frustum.planes[1] = Plane(matrix[3] - matrix[0], matrix[3][0] - matrix[0][0], matrix[3][1] - matrix[0][1], matrix[3][2] - matrix[0][2]); // право
        
        // Верхние и нижние плоскости
        frustum.planes[2] = Plane(matrix[3] - matrix[1], matrix[3][0] - matrix[1][0], matrix[3][1] - matrix[1][1], matrix[3][2] - matrix[1][2]); // верх
        frustum.planes[3] = Plane(matrix[3] + matrix[1], matrix[3][0] + matrix[1][0], matrix[3][1] + matrix[1][1], matrix[3][2] + matrix[1][2]); // низ
        
        // Ближние и дальние плоскости
        frustum.planes[4] = Plane(matrix[3] + matrix[2], matrix[3][0] + matrix[2][0], matrix[3][1] + matrix[2][1], matrix[3][2] + matrix[2][2]); // ближ
        frustum.planes[5] = Plane(matrix[3] - matrix[2], matrix[3][0] - matrix[2][0], matrix[3][1] - matrix[2][1], matrix[3][2] - matrix[2][2]); // даль
        
        // Нормализация нормалей плоскостей
        for (int i = 0; i < 6; i++) {
            real length = std::sqrt(frustum.planes[i].normal.x() * frustum.planes[i].normal.x() +
                                   frustum.planes[i].normal.y() * frustum.planes[i].normal.y() +
                                   frustum.planes[i].normal.z() * frustum.planes[i].normal.z());
            if (length > 0.0f) {
                frustum.planes[i].normal = Vector3(
                    frustum.planes[i].normal.x() / length,
                    frustum.planes[i].normal.y() / length,
                    frustum.planes[i].normal.z() / length
                );
                frustum.planes[i].distance /= length;
            }
        }
        
        return frustum;
    }
    
    // Проверка, находится ли точка внутри фрустума
    constexpr bool contains_point(const Vector3& point) const {
        for (int i = 0; i < 6; i++) {
            if (planes[i].distance_to_point(point) < 0.0f) {
                return false; // Точка за пределами хотя бы одной плоскости
            }
        }
        return true;
    }
    
    // Проверка пересечения AABB с фрустумом
    constexpr bool intersects_aabb(const AABB& box) const {
        for (int i = 0; i < 6; i++) {
            const Plane& plane = planes[i];
            Vector3 positive_vertex = box.min;
            
            // Выбираем вершину, наиболее удаленную от нормали плоскости
            if (plane.normal.x() >= 0) positive_vertex.x() = box.max.x();
            if (plane.normal.y() >= 0) positive_vertex.y() = box.max.y();
            if (plane.normal.z() >= 0) positive_vertex.z() = box.max.z();
            
            // Если наиболее удаленная вершина за пределами плоскости, то AABB вне фрустума
            if (plane.distance_to_point(positive_vertex) < 0.0f) {
                return false;
            }
        }
        return true;
    }
    
    // Проверка пересечения сферы с фрустумом
    constexpr bool intersects_sphere(const Sphere& sphere) const {
        for (int i = 0; i < 6; i++) {
            real distance = planes[i].distance_to_point(sphere.center);
            if (distance < -sphere.radius) {
                return false;  // Сфера полностью за пределами плоскости
            }
        }
        return true;
    }
};

// Определение константы для clamp
#ifndef MATH_LIB_CLAMP_DEFINED
#define MATH_LIB_CLAMP_DEFINED
namespace std {
    template<typename T>
    constexpr T clamp(T value, T low, T high) {
        return (value < low) ? low : (value > high) ? high : value;
    }
}
#endif
