#include "math_lib.hpp"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "Тестирование математической библиотеки для игр..." << std::endl;

    // Тест Vector3
    std::cout << "\nТест Vector3:" << std::endl;
    Vector3 v1(1.0f, 2.0f, 3.0f);
    Vector3 v2(4.0f, 5.0f, 6.0f);
    std::cout << "v1: (" << v1.x() << ", " << v1.y() << ", " << v1.z() << ")" << std::endl;
    std::cout << "v2: (" << v2.x() << ", " << v2.y() << ", " << v2.z() << ")" << std::endl;
    
    Vector3 v3 = v1 + v2;
    std::cout << "v1 + v2: (" << v3.x() << ", " << v3.y() << ", " << v3.z() << ")" << std::endl;
    
    Vector3 v4 = v1.cross(v2);
    std::cout << "v1 x v2: (" << v4.x() << ", " << v4.y() << ", " << v4.z() << ")" << std::endl;
    
    Vector3 v5 = v1.normalized();
    std::cout << "v1 нормализованный: (" << v5.x() << ", " << v5.y() << ", " << v5.z() << ")" << std::endl;

    // Тест Matrix4x4
    std::cout << "\nТест Matrix4x4:" << std::endl;
    Matrix4x4 m1 = Matrix4x4::rotation_x(math_constants::PI / 4.0f);
    std::cout << "Матрица поворота X на 45 градусов создана" << std::endl;
    
    Matrix4x4 m2 = Matrix4x4::translation(1.0f, 2.0f, 3.0f);
    std::cout << "Матрица трансляции (1, 2, 3) создана" << std::endl;
    
    Matrix4x4 m3 = m1 * m2;
    std::cout << "Результат умножения матриц вычислен" << std::endl;

    // Тест Quaternion
    std::cout << "\nТест Quaternion:" << std::endl;
    Vector3 axis(0.0f, 1.0f, 0.0f);
    Quaternion q(axis, math_constants::PI / 2.0f); // Поворот на 90 градусов вокруг Y
    std::cout << "Кватернион поворота: (" << q.x() << ", " << q.y() << ", " << q.z() << ", " << q.w() << ")" << std::endl;
    
    Vector3 rotated = q.rotate(Vector3(1.0f, 0.0f, 0.0f));
    std::cout << "Вектор (1,0,0) после поворота: (" << rotated.x() << ", " << rotated.y() << ", " << rotated.z() << ")" << std::endl;

    // Тест систем координат
    std::cout << "\nТест систем координат:" << std::endl;
    Vector3 eye(0.0f, 0.0f, 5.0f);
    Vector3 target(0.0f, 0.0f, 0.0f);
    Vector3 up(0.0f, 1.0f, 0.0f);
    
    Matrix4x4 look_at_lh = LeftHanded::look_at(eye, target, up);
    std::cout << "Матрица вида (левосторонняя): вычислена" << std::endl;
    
    Matrix4x4 look_at_rh = RightHanded::look_at(eye, target, up);
    std::cout << "Матрица вида (правосторонняя): вычислена" << std::endl;

    // Тест производительности
    std::cout << "\nТест производительности:" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Выполняем множество операций для тестирования производительности
    volatile float dummy = 0.0f;
    for(int i = 0; i < 100000; ++i) {
        Vector3 temp(i * 0.01f, i * 0.02f, i * 0.03f);
        temp = temp.normalized();
        dummy += temp.x() + temp.y() + temp.z();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время нормализации 100000 векторов: " << duration.count() << " микросекунд" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    
    for(int i = 0; i < 10000; ++i) {
        Matrix4x4 m = Matrix4x4::rotation_x(i * 0.01f) * Matrix4x4::translation(i * 0.01f, 0.0f, 0.0f);
        dummy += m[0][0] + m[1][1] + m[2][2];
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время умножения 10000 матриц: " << duration.count() << " микросекунд" << std::endl;
    
    std::cout << "\nТестирование завершено!" << std::endl;
    
    return 0;
}