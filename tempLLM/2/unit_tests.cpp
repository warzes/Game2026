#include "math_lib.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

// Макрос для проверки утверждений
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cout << "ПОЛОМАНО: " << message << " в " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

// Макрос для проверки равенства с погрешностью
#define ASSERT_FLOAT_EQ(a, b, epsilon) \
    do { \
        if (std::abs((a) - (b)) > (epsilon)) { \
            std::cout << "ПОЛОМАНО: " << #a << " != " << #b << " (" << (a) << " vs " << (b) << ") в " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

// Тесты для Vector2
bool test_vector2() {
    std::cout << "Тест Vector2..." << std::endl;
    
    // Тест конструкторов
    Vector2 v1;
    ASSERT(v1.x() == 0.0f && v1.y() == 0.0f, "Конструктор по умолчанию Vector2");
    
    Vector2 v2(3.0f, 4.0f);
    ASSERT(v2.x() == 3.0f && v2.y() == 4.0f, "Конструктор Vector2(x, y)");
    
    Vector2 v3(5.0f);
    ASSERT(v3.x() == 5.0f && v3.y() == 5.0f, "Конструктор Vector2(scalar)");
    
    // Тест длины
    ASSERT_FLOAT_EQ(v2.length(), 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v2.length_squared(), 25.0f, 1e-5f);
    
    // Тест нормализации
    Vector2 v2_norm = v2.normalized();
    ASSERT_FLOAT_EQ(v2_norm.length(), 1.0f, 1e-5f);
    
    // Тест скалярного произведения
    Vector2 v4(1.0f, 2.0f);
    ASSERT_FLOAT_EQ(v2.dot(v4), 11.0f, 1e-5f); // 3*1 + 4*2 = 11
    
    // Тест перпендикуляра
    Vector2 v2_perp = v2.perpendicular();
    ASSERT_FLOAT_EQ(v2_perp.x(), -4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v2_perp.y(), 3.0f, 1e-5f);
    
    // Тест операций
    Vector2 v5 = v2 + v4;
    ASSERT_FLOAT_EQ(v5.x(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v5.y(), 6.0f, 1e-5f);
    
    Vector2 v6 = v2 - v4;
    ASSERT_FLOAT_EQ(v6.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v6.y(), 2.0f, 1e-5f);
    
    Vector2 v7 = v2 * 2.0f;
    ASSERT_FLOAT_EQ(v7.x(), 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v7.y(), 8.0f, 1e-5f);
    
    Vector2 v8 = 2.0f * v2;
    ASSERT_FLOAT_EQ(v8.x(), 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v8.y(), 8.0f, 1e-5f);
    
    Vector2 v9 = v2 / 2.0f;
    ASSERT_FLOAT_EQ(v9.x(), 1.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v9.y(), 2.0f, 1e-5f);
    
    // Тест операторов присваивания
    Vector2 v10(1.0f, 1.0f);
    v10 += v2;
    ASSERT_FLOAT_EQ(v10.x(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.y(), 5.0f, 1e-5f);
    
    v10 -= v2;
    ASSERT_FLOAT_EQ(v10.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.y(), 1.0f, 1e-5f);
    
    v10 *= 3.0f;
    ASSERT_FLOAT_EQ(v10.x(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.y(), 3.0f, 1e-5f);
    
    v10 /= 3.0f;
    ASSERT_FLOAT_EQ(v10.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.y(), 1.0f, 1e-5f);
    
    // Тест оператора унарного минуса
    Vector2 v11 = -v2;
    ASSERT_FLOAT_EQ(v11.x(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v11.y(), -4.0f, 1e-5f);
    
    // Тест линейной интерполяции
    Vector2 v12 = v2.lerp(v4, 0.5f);
    ASSERT_FLOAT_EQ(v12.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 3.0f, 1e-5f);
    
    std::cout << "Vector2 тест пройден!" << std::endl;
    return true;
}

// Тесты для Vector3
bool test_vector3() {
    std::cout << "Тест Vector3..." << std::endl;
    
    // Тест конструкторов
    Vector3 v1;
    ASSERT(v1.x() == 0.0f && v1.y() == 0.0f && v1.z() == 0.0f, "Конструктор по умолчанию Vector3");
    
    Vector3 v2(1.0f, 2.0f, 3.0f);
    ASSERT(v2.x() == 1.0f && v2.y() == 2.0f && v2.z() == 3.0f, "Конструктор Vector3(x, y, z)");
    
    Vector3 v3(5.0f);
    ASSERT(v3.x() == 5.0f && v3.y() == 5.0f && v3.z() == 5.0f, "Конструктор Vector3(scalar)");
    
    Vector3 v4(Vector2(1.0f, 2.0f), 3.0f);
    ASSERT(v4.x() == 1.0f && v4.y() == 2.0f && v4.z() == 3.0f, "Конструктор Vector3(Vector2, z)");
    
    // Тест длины
    ASSERT_FLOAT_EQ(v2.length(), std::sqrt(14.0f), 1e-5f);
    ASSERT_FLOAT_EQ(v2.length_squared(), 14.0f, 1e-5f);
    
    // Тест нормализации
    Vector3 v2_norm = v2.normalized();
    ASSERT_FLOAT_EQ(v2_norm.length(), 1.0f, 1e-5f);
    
    // Тест скалярного произведения
    Vector3 v5(4.0f, 5.0f, 6.0f);
    ASSERT_FLOAT_EQ(v2.dot(v5), 32.0f, 1e-5f); // 1*4 + 2*5 + 3*6 = 32
    
    // Тест векторного произведения
    Vector3 v6 = v2.cross(v5);
    ASSERT_FLOAT_EQ(v6.x(), -3.0f, 1e-5f); // 2*6 - 3*5
    ASSERT_FLOAT_EQ(v6.y(), 6.0f, 1e-5f); // 3*4 - 1*6
    ASSERT_FLOAT_EQ(v6.z(), -3.0f, 1e-5f); // 1*5 - 2*4
    
    // Тест операций
    Vector3 v7 = v2 + v5;
    ASSERT_FLOAT_EQ(v7.x(), 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v7.y(), 7.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v7.z(), 9.0f, 1e-5f);
    
    Vector3 v8 = v2 - v5;
    ASSERT_FLOAT_EQ(v8.x(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v8.y(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v8.z(), -3.0f, 1e-5f);
    
    Vector3 v9 = v2 * 2.0f;
    ASSERT_FLOAT_EQ(v9.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v9.y(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v9.z(), 6.0f, 1e-5f);
    
    Vector3 v10 = 2.0f * v2;
    ASSERT_FLOAT_EQ(v10.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.y(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.z(), 6.0f, 1e-5f);
    
    Vector3 v11 = v2 / 2.0f;
    ASSERT_FLOAT_EQ(v11.x(), 0.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v11.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v11.z(), 1.5f, 1e-5f);
    
    // Тест операторов присваивания
    Vector3 v12(1.0f, 1.0f, 1.0f);
    v12 += v2;
    ASSERT_FLOAT_EQ(v12.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 4.0f, 1e-5f);
    
    v12 -= v2;
    ASSERT_FLOAT_EQ(v12.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 1.0f, 1e-5f);
    
    v12 *= 3.0f;
    ASSERT_FLOAT_EQ(v12.x(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 3.0f, 1e-5f);
    
    v12 /= 3.0f;
    ASSERT_FLOAT_EQ(v12.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 1.0f, 1e-5f);
    
    // Тест оператора унарного минуса
    Vector3 v13 = -v2;
    ASSERT_FLOAT_EQ(v13.x(), -1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v13.y(), -2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v13.z(), -3.0f, 1e-5f);
    
    // Тест линейной интерполяции
    Vector3 v14 = v2.lerp(v5, 0.5f);
    ASSERT_FLOAT_EQ(v14.x(), 2.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v14.y(), 3.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v14.z(), 4.5f, 1e-5f);
    
    // Тест получения Vector2
    Vector2 v15 = v2.xy();
    ASSERT_FLOAT_EQ(v15.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v15.y(), 2.0f, 1e-5f);
    
    std::cout << "Vector3 тест пройден!" << std::endl;
    return true;
}

// Тесты для Vector4
bool test_vector4() {
    std::cout << "Тест Vector4..." << std::endl;
    
    // Тест конструкторов
    Vector4 v1;
    ASSERT(v1.x() == 0.0f && v1.y() == 0.0f && v1.z() == 0.0f && v1.w() == 0.0f, "Конструктор по умолчанию Vector4");
    
    Vector4 v2(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT(v2.x() == 1.0f && v2.y() == 2.0f && v2.z() == 3.0f && v2.w() == 4.0f, "Конструктор Vector4(x, y, z, w)");
    
    Vector4 v3(5.0f);
    ASSERT(v3.x() == 5.0f && v3.y() == 5.0f && v3.z() == 5.0f && v3.w() == 5.0f, "Конструктор Vector4(scalar)");
    
    Vector4 v4(Vector3(1.0f, 2.0f, 3.0f), 4.0f);
    ASSERT(v4.x() == 1.0f && v4.y() == 2.0f && v4.z() == 3.0f && v4.w() == 4.0f, "Конструктор Vector4(Vector3, w)");
    
    Vector4 v5(Vector2(1.0f, 2.0f), 3.0f, 4.0f);
    ASSERT(v5.x() == 1.0f && v5.y() == 2.0f && v5.z() == 3.0f && v5.w() == 4.0f, "Конструктор Vector4(Vector2, z, w)");
    
    // Тест длины
    ASSERT_FLOAT_EQ(v2.length(), std::sqrt(30.0f), 1e-5f);
    ASSERT_FLOAT_EQ(v2.length_squared(), 30.0f, 1e-5f);
    
    // Тест нормализации
    Vector4 v2_norm = v2.normalized();
    ASSERT_FLOAT_EQ(v2_norm.length(), 1.0f, 1e-5f);
    
    // Тест скалярного произведения
    Vector4 v6(4.0f, 5.0f, 6.0f, 7.0f);
    ASSERT_FLOAT_EQ(v2.dot(v6), 50.0f, 1e-5f); // 1*4 + 2*5 + 3*6 + 4*7 = 50
    
    // Тест операций
    Vector4 v7 = v2 + v6;
    ASSERT_FLOAT_EQ(v7.x(), 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v7.y(), 7.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v7.z(), 9.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v7.w(), 11.0f, 1e-5f);
    
    Vector4 v8 = v2 - v6;
    ASSERT_FLOAT_EQ(v8.x(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v8.y(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v8.z(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v8.w(), -3.0f, 1e-5f);
    
    Vector4 v9 = v2 * 2.0f;
    ASSERT_FLOAT_EQ(v9.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v9.y(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v9.z(), 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v9.w(), 8.0f, 1e-5f);
    
    Vector4 v10 = 2.0f * v2;
    ASSERT_FLOAT_EQ(v10.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.y(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.z(), 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v10.w(), 8.0f, 1e-5f);
    
    Vector4 v11 = v2 / 2.0f;
    ASSERT_FLOAT_EQ(v11.x(), 0.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v11.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v11.z(), 1.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v11.w(), 2.0f, 1e-5f);
    
    // Тест операторов присваивания
    Vector4 v12(1.0f, 1.0f, 1.0f, 1.0f);
    v12 += v2;
    ASSERT_FLOAT_EQ(v12.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.w(), 5.0f, 1e-5f);
    
    v12 -= v2;
    ASSERT_FLOAT_EQ(v12.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.w(), 1.0f, 1e-5f);
    
    v12 *= 3.0f;
    ASSERT_FLOAT_EQ(v12.x(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.w(), 3.0f, 1e-5f);
    
    v12 /= 3.0f;
    ASSERT_FLOAT_EQ(v12.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.z(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v12.w(), 1.0f, 1e-5f);
    
    // Тест оператора унарного минуса
    Vector4 v13 = -v2;
    ASSERT_FLOAT_EQ(v13.x(), -1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v13.y(), -2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v13.z(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v13.w(), -4.0f, 1e-5f);
    
    // Тест линейной интерполяции
    Vector4 v14 = v2.lerp(v6, 0.5f);
    ASSERT_FLOAT_EQ(v14.x(), 2.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v14.y(), 3.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v14.z(), 4.5f, 1e-5f);
    ASSERT_FLOAT_EQ(v14.w(), 5.5f, 1e-5f);
    
    // Тест получения Vector3
    Vector3 v15 = v2.xyz();
    ASSERT_FLOAT_EQ(v15.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v15.y(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v15.z(), 3.0f, 1e-5f);
    
    // Тест получения Vector2
    Vector2 v16 = v2.xy();
    ASSERT_FLOAT_EQ(v16.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(v16.y(), 2.0f, 1e-5f);
    
    std::cout << "Vector4 тест пройден!" << std::endl;
    return true;
}

// Тесты для Matrix3x3
bool test_matrix3x3() {
    std::cout << "Тест Matrix3x3..." << std::endl;
    
    // Тест конструктора единичной матрицы
    Matrix3x3 m1;
    ASSERT_FLOAT_EQ(m1[0][0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[1][1], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[2][2], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[0][1], 0.0f, 1e-5f);
    
    // Тест конструктора с параметрами
    Matrix3x3 m2(1, 2, 3, 4, 5, 6, 7, 8, 9);
    ASSERT_FLOAT_EQ(m2[0][0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[0][1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[0][2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][0], 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][1], 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][2], 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][0], 7.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][1], 8.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][2], 9.0f, 1e-5f);
    
    // Тест умножения матриц
    Matrix3x3 m3(1, 0, 0, 0, 1, 0, 0, 0, 1); // единичная
    Matrix3x3 m4 = m2 * m3;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            ASSERT_FLOAT_EQ(m4[i][j], m2[i][j], 1e-5f);
        }
    }
    
    // Тест умножения на вектор
    Vector3 v(1, 2, 3);
    Vector3 result = m3 * v; // умножение на единичную матрицу
    ASSERT_FLOAT_EQ(result.x(), v.x(), 1e-5f);
    ASSERT_FLOAT_EQ(result.y(), v.y(), 1e-5f);
    ASSERT_FLOAT_EQ(result.z(), v.z(), 1e-5f);
    
    // Тест транспонирования
    Matrix3x3 m5 = m2.transposed();
    ASSERT_FLOAT_EQ(m5[0][1], m2[1][0], 1e-5f);
    ASSERT_FLOAT_EQ(m5[1][0], m2[0][1], 1e-5f);
    
    // Тест определителя
    Matrix3x3 identity;
    ASSERT_FLOAT_EQ(identity.determinant(), 1.0f, 1e-5f);
    
    // Тест инверсии
    Matrix3x3 m6 = m2.inverted();
    Matrix3x3 m7 = m2 * m6;
    // Проверяем, что m2 * m2^(-1) ≈ I
    ASSERT_FLOAT_EQ(m7[0][0], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[1][1], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[2][2], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[0][1], 0.0f, 1e-3f);
    
    // Тест создания матрицы поворота Z
    Matrix3x3 rot_z = Matrix3x3::rotation_z(math_constants::PI / 2.0f); // 90 градусов
    Vector3 test_v(1, 0, 0);
    Vector3 rotated = rot_z * test_v;
    ASSERT_FLOAT_EQ(rotated.x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated.y(), 1.0f, 1e-5f);
    
    // Тест масштабирующей матрицы
    Matrix3x3 scale = Matrix3x3::scale(2.0f, 3.0f, 4.0f);
    Vector3 scale_test(1, 1, 1);
    Vector3 scaled = scale * scale_test;
    ASSERT_FLOAT_EQ(scaled.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(scaled.y(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(scaled.z(), 4.0f, 1e-5f);
    
    std::cout << "Matrix3x3 тест пройден!" << std::endl;
    return true;
}

// Тесты для Matrix4x4
bool test_matrix4x4() {
    std::cout << "Тест Matrix4x4..." << std::endl;
    
    // Тест конструктора единичной матрицы
    Matrix4x4 m1;
    ASSERT_FLOAT_EQ(m1[0][0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[1][1], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[2][2], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[3][3], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m1[0][1], 0.0f, 1e-5f);
    
    // Тест конструктора с параметрами
    Matrix4x4 m2(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    ASSERT_FLOAT_EQ(m2[0][0], 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[0][1], 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[0][2], 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[0][3], 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][0], 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][1], 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][2], 7.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[1][3], 8.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][0], 9.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][1], 10.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][2], 11.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[2][3], 12.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[3][0], 13.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[3][1], 14.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[3][2], 15.0f, 1e-5f);
    ASSERT_FLOAT_EQ(m2[3][3], 16.0f, 1e-5f);
    
    // Тест умножения матриц
    Matrix4x4 m3(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1); // единичная
    Matrix4x4 m4 = m2 * m3;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            ASSERT_FLOAT_EQ(m4[i][j], m2[i][j], 1e-5f);
        }
    }
    
    // Тест умножения на Vector4
    Vector4 v4(1, 2, 3, 4);
    Vector4 result4 = m3 * v4; // умножение на единичную матрицу
    ASSERT_FLOAT_EQ(result4.x(), v4.x(), 1e-5f);
    ASSERT_FLOAT_EQ(result4.y(), v4.y(), 1e-5f);
    ASSERT_FLOAT_EQ(result4.z(), v4.z(), 1e-5f);
    ASSERT_FLOAT_EQ(result4.w(), v4.w(), 1e-5f);
    
    // Тест умножения на Vector3 (положение)
    Vector3 v3(1, 2, 3);
    Vector3 result3 = m3 * v3; // умножение на единичную матрицу
    ASSERT_FLOAT_EQ(result3.x(), v3.x(), 1e-5f);
    ASSERT_FLOAT_EQ(result3.y(), v3.y(), 1e-5f);
    ASSERT_FLOAT_EQ(result3.z(), v3.z(), 1e-5f);
    
    // Тест транспонирования
    Matrix4x4 m5 = m2.transposed();
    ASSERT_FLOAT_EQ(m5[0][1], m2[1][0], 1e-5f);
    ASSERT_FLOAT_EQ(m5[1][0], m2[0][1], 1e-5f);
    
    // Тест определителя
    Matrix4x4 identity;
    ASSERT_FLOAT_EQ(identity.determinant(), 1.0f, 1e-5f);
    
    // Тест инверсии
    Matrix4x4 m6 = m2.inverted();
    Matrix4x4 m7 = m2 * m6;
    // Проверяем, что m2 * m2^(-1) ≈ I
    ASSERT_FLOAT_EQ(m7[0][0], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[1][1], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[2][2], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[3][3], 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(m7[0][1], 0.0f, 1e-3f);
    
    // Тест поворота X
    Matrix4x4 rot_x = Matrix4x4::rotation_x(math_constants::PI / 2.0f); // 90 градусов
    Vector3 test_v_x(1, 0, 0);
    Vector3 rotated_x = rot_x * test_v_x;
    ASSERT_FLOAT_EQ(rotated_x.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated_x.y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated_x.z(), 0.0f, 1e-5f);
    
    // Тест поворота Y
    Matrix4x4 rot_y = Matrix4x4::rotation_y(math_constants::PI / 2.0f); // 90 градусов
    Vector3 test_v_y(1, 0, 0);
    Vector3 rotated_y = rot_y * test_v_y;
    ASSERT_FLOAT_EQ(rotated_y.x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated_y.y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated_y.z(), -1.0f, 1e-5f);
    
    // Тест поворота Z
    Matrix4x4 rot_z = Matrix4x4::rotation_z(math_constants::PI / 2.0f); // 90 градусов
    Vector3 test_v_z(1, 0, 0);
    Vector3 rotated_z = rot_z * test_v_z;
    ASSERT_FLOAT_EQ(rotated_z.x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated_z.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated_z.z(), 0.0f, 1e-5f);
    
    // Тест трансляции
    Matrix4x4 trans = Matrix4x4::translation(5.0f, 3.0f, 2.0f);
    Vector3 trans_v(1, 1, 1);
    Vector3 trans_result = trans * trans_v;
    ASSERT_FLOAT_EQ(trans_result.x(), 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(trans_result.y(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(trans_result.z(), 3.0f, 1e-5f);
    
    // Тест масштабирования
    Matrix4x4 scale = Matrix4x4::scale(2.0f, 3.0f, 4.0f);
    Vector3 scale_v(1, 1, 1);
    Vector3 scale_result = scale * scale_v;
    ASSERT_FLOAT_EQ(scale_result.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(scale_result.y(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(scale_result.z(), 4.0f, 1e-5f);
    
    // Тест перспективной проекции
    Matrix4x4 proj_lh = Matrix4x4::perspective_lh(math_constants::PI / 2.0f, 1.0f, 0.1f, 100.0f);
    Matrix4x4 proj_rh = Matrix4x4::perspective_rh(math_constants::PI / 2.0f, 1.0f, 0.1f, 100.0f);
    // Проверим, что это разные матрицы
    ASSERT(proj_lh[2][2] != proj_rh[2][2] || proj_lh[2][3] != proj_rh[2][3], "Левая и правая перспективные матрицы должны отличаться");
    
    // Тест ортографической проекции
    Matrix4x4 ortho_lh = Matrix4x4::orthographic_lh(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    Matrix4x4 ortho_rh = Matrix4x4::orthographic_rh(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
    // Проверим, что это разные матрицы
    ASSERT(ortho_lh[2][2] != ortho_rh[2][2], "Левая и правая ортографические матрицы должны отличаться");
    
    // Тест look_at
    Vector3 eye(0, 0, 5);
    Vector3 target(0, 0, 0);
    Vector3 up(0, 1, 0);
    Matrix4x4 lookat_lh = Matrix4x4::look_at_lh(eye, target, up);
    Matrix4x4 lookat_rh = Matrix4x4::look_at_rh(eye, target, up);
    // Проверим, что это разные матрицы
    ASSERT(lookat_lh[2][0] != lookat_rh[2][0] || lookat_lh[2][1] != lookat_rh[2][1] || lookat_lh[2][2] != lookat_rh[2][2], 
           "Левая и правая look-at матрицы должны отличаться");
    
    std::cout << "Matrix4x4 тест пройден!" << std::endl;
    return true;
}

// Тесты для Quaternion
bool test_quaternion() {
    std::cout << "Тест Quaternion..." << std::endl;
    
    // Тест конструктора по умолчанию (единичный кватернион)
    Quaternion q1;
    ASSERT_FLOAT_EQ(q1.x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q1.y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q1.z(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q1.w(), 1.0f, 1e-5f);
    
    // Тест конструктора с параметрами
    Quaternion q2(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_FLOAT_EQ(q2.x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q2.y(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q2.z(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q2.w(), 4.0f, 1e-5f);
    
    // Тест конструктора из оси и угла
    Vector3 axis(0, 1, 0); // ось Y
    Quaternion q3(axis, math_constants::PI / 2.0f); // поворот на 90 градусов
    // Для поворота на 90 градусов вокруг Y: q = [0, sin(45°), 0, cos(45°)]
    ASSERT_FLOAT_EQ(q3.w(), std::cos(math_constants::PI / 4.0f), 1e-5f);
    ASSERT_FLOAT_EQ(q3.y(), std::sin(math_constants::PI / 4.0f), 1e-5f);
    
    // Тест длины
    ASSERT_FLOAT_EQ(q1.length(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q2.length_squared(), 30.0f, 1e-5f); // 1^2 + 2^2 + 3^2 + 4^2 = 30
    
    // Тест нормализации
    Quaternion q2_norm = q2.normalized();
    ASSERT_FLOAT_EQ(q2_norm.length(), 1.0f, 1e-5f);
    
    // Тест сопряжения
    Quaternion q4 = q2.conjugated();
    ASSERT_FLOAT_EQ(q4.x(), -1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q4.y(), -2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q4.z(), -3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q4.w(), 4.0f, 1e-5f);
    
    // Тест инверсии
    Quaternion q5 = q2.inverted();
    Quaternion q6 = q2 * q5;
    // q * q^(-1) должно быть близко к единичному кватерниону
    ASSERT_FLOAT_EQ(q6.w(), 1.0f, 1e-3f);
    ASSERT_FLOAT_EQ(q6.x(), 0.0f, 1e-3f);
    ASSERT_FLOAT_EQ(q6.y(), 0.0f, 1e-3f);
    ASSERT_FLOAT_EQ(q6.z(), 0.0f, 1e-3f);
    
    // Тест поворота вектора
    Vector3 v(1, 0, 0); // вектор X
    // Поворот на 90 градусов вокруг Y должен дать вектор -Z
    Vector3 rotated = q3.rotate(v);
    ASSERT_FLOAT_EQ(rotated.x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated.y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(rotated.z(), -1.0f, 1e-5f);
    
    // Тест умножения кватернионов
    Quaternion q7(0, 0, 0, 1); // единичный
    Quaternion q8 = q2 * q7;
    ASSERT_FLOAT_EQ(q8.x(), q2.x(), 1e-5f);
    ASSERT_FLOAT_EQ(q8.y(), q2.y(), 1e-5f);
    ASSERT_FLOAT_EQ(q8.z(), q2.z(), 1e-5f);
    ASSERT_FLOAT_EQ(q8.w(), q2.w(), 1e-5f);
    
    // Тест умножения на скаляр
    Quaternion q9 = q2 * 2.0f;
    ASSERT_FLOAT_EQ(q9.x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q9.y(), 4.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q9.z(), 6.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q9.w(), 8.0f, 1e-5f);
    
    // Тест скалярного произведения
    Quaternion q10(1.0f, 0.0f, 0.0f, 0.0f);
    ASSERT_FLOAT_EQ(q2.dot(q10), 1.0f, 1e-5f);
    
    // Тест LERP
    Quaternion q11 = q1.lerp(q2, 0.5f);
    ASSERT_FLOAT_EQ(q11.x(), 0.5f, 1e-5f);
    ASSERT_FLOAT_EQ(q11.y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(q11.z(), 1.5f, 1e-5f);
    ASSERT_FLOAT_EQ(q11.w(), 2.5f, 1e-5f);
    
    // Тест SLERP
    // SLERP между единичным кватернионом и другим на t=0.5
    Quaternion q12 = q1.slerp(q3, 0.5f);
    // Результат должен быть поворотом на 45 градусов вокруг Y
    ASSERT_FLOAT_EQ(q12.w(), std::cos(math_constants::PI / 8.0f), 1e-5f);
    ASSERT_FLOAT_EQ(q12.y(), std::sin(math_constants::PI / 8.0f), 1e-5f);
    
    // Тест from_euler_angles
    Quaternion q13 = Quaternion::from_euler_angles(0.0f, math_constants::PI / 2.0f, 0.0f); // поворот на 90 градусов по Y
    // Должно быть близко к q3
    ASSERT_FLOAT_EQ(q13.w(), q3.w(), 1e-5f);
    ASSERT_FLOAT_EQ(q13.x(), q3.x(), 1e-5f);
    ASSERT_FLOAT_EQ(q13.y(), q3.y(), 1e-5f);
    ASSERT_FLOAT_EQ(q13.z(), q3.z(), 1e-5f);
    
    std::cout << "Quaternion тест пройден!" << std::endl;
    return true;
}

// Тесты для систем координат
bool test_coord_systems() {
    std::cout << "Тест систем координат..." << std::endl;
    
    Vector3 eye(0, 0, 5);
    Vector3 target(0, 0, 0);
    Vector3 up(0, 1, 0);
    
    Matrix4x4 look_at_lh = LeftHanded::look_at(eye, target, up);
    Matrix4x4 look_at_rh = RightHanded::look_at(eye, target, up);
    
    // Проверим, что они отличаются
    bool different = false;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(std::abs(look_at_lh[i][j] - look_at_rh[i][j]) > 1e-5f) {
                different = true;
                break;
            }
        }
        if(different) break;
    }
    ASSERT(different, "Левая и правая системы координат должны давать разные матрицы вида");
    
    // Тест перспективной проекции
    Matrix4x4 proj_lh = LeftHanded::perspective(math_constants::PI / 2.0f, 1.0f, 0.1f, 100.0f);
    Matrix4x4 proj_rh = RightHanded::perspective(math_constants::PI / 2.0f, 1.0f, 0.1f, 10.0f);
    
    different = false;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(std::abs(proj_lh[i][j] - proj_rh[i][j]) > 1e-5f) {
                different = true;
                break;
            }
        if(different) break;
    }
    ASSERT(different, "Левая и правая системы координат должны давать разные перспективные матрицы");
    
    // Тест ортографической проекции
    Matrix4x4 ortho_lh = LeftHanded::orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
    Matrix4x4 ortho_rh = RightHanded::orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
    
    different = false;
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(std::abs(ortho_lh[i][j] - ortho_rh[i][j]) > 1e-5f) {
                different = true;
                break;
            }
        }
        if(different) break;
    }
    ASSERT(different, "Левая и правая системы координат должны давать разные ортографические матрицы");
    
    std::cout << "Тест систем координат пройден!" << std::endl;
    return true;
}

// Тест производительности
void performance_test() {
    std::cout << "\nТест производительности..." << std::endl;
    
    const int iterations = 100000;
    
    // Тест производительности векторных операций
    auto start = std::chrono::high_resolution_clock::now();
    volatile float dummy = 0.0f;
    for(int i = 0; i < iterations; ++i) {
        Vector3 v1(i * 0.001f, i * 0.0002f, i * 0.003f);
        Vector3 v2(i * 0.0004f, i * 0.0005f, i * 0.0006f);
        Vector3 result = v1 + v2;
        dummy += result.x() + result.y() + result.z();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время для " << iterations << " векторных сложений: " << duration.count() << " микросекунд" << std::endl;
    
    // Тест производительности нормализации
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < iterations/10; ++i) { // 10 раз меньше из-за sqrt
        Vector3 temp(i * 0.01f, i * 0.02f, i * 0.03f);
        temp = temp.normalized();
        dummy += temp.x() + temp.y() + temp.z();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время для " << iterations/10 << " нормализаций векторов: " << duration.count() << " микросекунд" << std::endl;
    
    // Тест производительности матричных операций
    const int matrix_iterations = 1000;
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < matrix_iterations; ++i) {
        Matrix4x4 m1 = Matrix4x4::rotation_x(i * 0.0001f);
        Matrix4x4 m2 = Matrix4x4::translation(i * 0.0001f, 0.0f, 0.0f);
        Matrix4x4 result = m1 * m2;
        dummy += result[0][0] + result[1][1] + result[2][2];
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время для " << matrix_iterations << " умножений матриц: " << duration.count() << " микросекунд" << std::endl;
    
    // Тест производительности кватернионных операций
    const int quat_iterations = 10000;
    start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < quat_iterations; ++i) {
        Quaternion q1(0.1f, 0.2f, 0.3f, 0.4f);
        Quaternion q2(0.4f, 0.3f, 0.2f, 0.1f);
        Quaternion result = q1 * q2;
        dummy += result.x() + result.y() + result.z() + result.w();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время для " << quat_iterations << " умножений кватернионов: " << duration.count() << " микросекунд" << std::endl;
}

int main() {
    std::cout << "=== Запуск юнит-тестов математической библиотеки ===" << std::endl;
    
    bool all_passed = true;
    
    all_passed &= test_vector2();
    all_passed &= test_vector3();
    all_passed &= test_vector4();
    all_passed &= test_matrix3x3();
    all_passed &= test_matrix4x4();
    all_passed &= test_quaternion();
    all_passed &= test_coord_systems();
    
    if(all_passed) {
        std::cout << "\n=== Все юнит-тесты пройдены успешно! ===" << std::endl;
        
        // Запуск теста производительности
        performance_test();
    } else {
        std::cout << "\n=== Некоторые тесты провалены! ===" << std::endl;
        return 1;
    }
    
    return 0;
}