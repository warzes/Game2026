#include "../src/handles.hpp"
#include <gtest/gtest.h>

// Тесты для хендлов ресурсов
class HandlesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Установка начального состояния для тестов
    }

    void TearDown() override {
        // Очистка после тестов
    }
};

// Тестирование BufferHandle
TEST_F(HandlesTest, BufferHandleInitialization) {
    BufferHandle default_handle;
    EXPECT_FALSE(default_handle.valid());
    EXPECT_EQ(default_handle.handle, 0u);
    
    BufferHandle initialized_handle(123);
    EXPECT_TRUE(initialized_handle.valid());
    EXPECT_EQ(initialized_handle.handle, 123u);
}

TEST_F(HandlesTest, BufferHandleComparison) {
    BufferHandle handle1(100);
    BufferHandle handle2(100);
    BufferHandle handle3(200);
    
    EXPECT_EQ(handle1, handle2);
    EXPECT_NE(handle1, handle3);
    EXPECT_LT(handle1, handle3);
}

// Тестирование TextureHandle
TEST_F(HandlesTest, TextureHandleInitialization) {
    TextureHandle default_handle;
    EXPECT_FALSE(default_handle.valid());
    EXPECT_EQ(default_handle.handle, 0u);
    
    TextureHandle initialized_handle(456);
    EXPECT_TRUE(initialized_handle.valid());
    EXPECT_EQ(initialized_handle.handle, 456u);
}

TEST_F(HandlesTest, TextureHandleComparison) {
    TextureHandle handle1(100);
    TextureHandle handle2(100);
    TextureHandle handle3(200);
    
    EXPECT_EQ(handle1, handle2);
    EXPECT_NE(handle1, handle3);
    EXPECT_LT(handle1, handle3);
}

// Тестирование ShaderHandle
TEST_F(HandlesTest, ShaderHandleInitialization) {
    ShaderHandle default_handle;
    EXPECT_FALSE(default_handle.valid());
    EXPECT_EQ(default_handle.handle, 0u);
    
    ShaderHandle initialized_handle(789);
    EXPECT_TRUE(initialized_handle.valid());
    EXPECT_EQ(initialized_handle.handle, 789u);
}

// Тестирование ProgramHandle
TEST_F(HandlesTest, ProgramHandleInitialization) {
    ProgramHandle default_handle;
    EXPECT_FALSE(default_handle.valid());
    EXPECT_EQ(default_handle.handle, 0u);
    
    ProgramHandle initialized_handle(101);
    EXPECT_TRUE(initialized_handle.valid());
    EXPECT_EQ(initialized_handle.handle, 101u);
}

// Тестирование VertexArrayHandle
TEST_F(HandlesTest, VertexArrayHandleInitialization) {
    VertexArrayHandle default_handle;
    EXPECT_FALSE(default_handle.valid());
    EXPECT_EQ(default_handle.handle, 0u);
    
    VertexArrayHandle initialized_handle(202);
    EXPECT_TRUE(initialized_handle.valid());
    EXPECT_EQ(initialized_handle.handle, 202u);
}

// Тестирование спецификаторов вершинных атрибутов
TEST_F(HandlesTest, VertexAttribBindingInitialization) {
    VertexAttribBinding default_binding;
    EXPECT_EQ(default_binding.index, 0u);
    
    VertexAttribBinding initialized_binding(5);
    EXPECT_EQ(initialized_binding.index, 5u);
}

TEST_F(HandlesTest, VertexAttribBindingComparison) {
    VertexAttribBinding binding1(3);
    VertexAttribBinding binding2(3);
    VertexAttribBinding binding3(7);
    
    EXPECT_EQ(binding1, binding2);
    EXPECT_NE(binding1, binding3);
    EXPECT_LT(binding1, binding3);
}

// Тестирование VertexBufferBinding
TEST_F(HandlesTest, VertexBufferBindingInitialization) {
    VertexBufferBinding default_binding;
    EXPECT_EQ(default_binding.index, 0u);
    
    VertexBufferBinding initialized_binding(10);
    EXPECT_EQ(initialized_binding.index, 10u);
}

TEST_F(HandlesTest, VertexBufferBindingComparison) {
    VertexBufferBinding binding1(5);
    VertexBufferBinding binding2(5);
    VertexBufferBinding binding3(15);
    
    EXPECT_EQ(binding1, binding2);
    EXPECT_NE(binding1, binding3);
    EXPECT_LT(binding1, binding3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}