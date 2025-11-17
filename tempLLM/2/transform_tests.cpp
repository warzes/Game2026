#include "math_lib.hpp"
#include <iostream>

// Макрос для проверки утверждений
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cout << "ПОЛОМАНО: " << message << " на строке " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

// Макрос для проверки равенства с погрешностью
#define ASSERT_FLOAT_EQ(a, b, epsilon) \
    do { \
        if (std::abs((a) - (b)) > (epsilon)) { \
            std::cout << "ПОЛОМАНО: " << #a << " != " << #b << " (" << (a) << " vs " << (b) << ") на строке " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

// Тестирование базовой функциональности Transform
bool test_transform_basic() {
    std::cout << "Тестирование базовой функциональности Transform..." << std::endl;
    
    Transform t;
    
    // Проверка начальных значений
    ASSERT_FLOAT_EQ(t.get_position().x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_position().y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_position().z(), 0.0f, 1e-5f);
    
    ASSERT_FLOAT_EQ(t.get_rotation().x(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_rotation().y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_rotation().z(), 0.0f, 1e-5f);
    
    ASSERT_FLOAT_EQ(t.get_scale().x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_scale().y(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_scale().z(), 1.0f, 1e-5f);
    
    ASSERT(t.get_parent() == nullptr, "Родитель должен быть nullptr по умолчанию");
    
    // Тестирование установки значений
    t.set_position(1.0f, 2.0f, 3.0f);
    ASSERT_FLOAT_EQ(t.get_position().x(), 1.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_position().y(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_position().z(), 3.0f, 1e-5f);
    
    t.set_rotation(45.0f, 90.0f, 180.0f);
    ASSERT_FLOAT_EQ(t.get_rotation().x(), 45.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_rotation().y(), 90.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_rotation().z(), 180.0f, 1e-5f);
    
    t.set_scale(2.0f, 0.5f, 3.0f);
    ASSERT_FLOAT_EQ(t.get_scale().x(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_scale().y(), 0.5f, 1e-5f);
    ASSERT_FLOAT_EQ(t.get_scale().z(), 3.0f, 1e-5f);
    
    std::cout << "Базовая функциональность Transform протестирована!" << std::endl;
    return true;
}

// Тестирование локальных и глобальных преобразований
bool test_transform_hierarchy() {
    std::cout << "Тестирование иерархии Transform..." << std::endl;
    
    Transform parent;
    Transform child;
    
    // Установим родителя
    child.set_parent(&parent);
    ASSERT(child.get_parent() == &parent, "Родитель должен быть установлен");
    
    // Установим позиции
    parent.set_position(10.0f, 0.0f, 0.0f);
    child.set_position(5.0f, 0.0f, 0.0f);
    
    // Проверим глобальную позицию
    Vector3 global_pos = child.get_global_position();
    ASSERT_FLOAT_EQ(global_pos.x(), 15.0f, 1e-5f);
    ASSERT_FLOAT_EQ(global_pos.y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(global_pos.z(), 0.0f, 1e-5f);
    
    // Проверим локальную позицию
    Vector3 local_pos = child.get_position();
    ASSERT_FLOAT_EQ(local_pos.x(), 5.0f, 1e-5f);
    ASSERT_FLOAT_EQ(local_pos.y(), 0.0f, 1e-5f);
    ASSERT_FLOAT_EQ(local_pos.z(), 0.0f, 1e-5f);
    
    // Проверим масштаб
    parent.set_scale(2.0f, 2.0f, 2.0f);
    child.set_scale(1.5f, 1.5f, 1.5f);
    
    Vector3 global_scale = child.get_global_scale();
    ASSERT_FLOAT_EQ(global_scale.x(), 3.0f, 1e-5f);  // 2.0 * 1.5
    ASSERT_FLOAT_EQ(global_scale.y(), 3.0f, 1e-5f);  // 2.0 * 1.5
    ASSERT_FLOAT_EQ(global_scale.z(), 3.0f, 1e-5f); // 2.0 * 1.5
    
    std::cout << "Иерархия Transform протестирована!" << std::endl;
    return true;
}

// Тестирование матриц трансформации
bool test_transform_matrices() {
    std::cout << "Тестирование матриц трансформации..." << std::endl;
    
    Transform t;
    t.set_position(1.0f, 2.0f, 3.0f);
    t.set_scale(2.0f, 2.0f, 2.0f);
    
    // Проверим, что матрица создается без ошибок
    const Matrix4x4& local_matrix = t.get_local_matrix();
    const Matrix4x4& world_matrix = t.get_world_matrix();
    
    // Проверим, что матрицы не равны нулю
    ASSERT_FLOAT_EQ(local_matrix[3][0], 1.0f, 1e-5f); // x-компонента позиции
    ASSERT_FLOAT_EQ(local_matrix[3][1], 2.0f, 1e-5f); // y-компонента позиции
    ASSERT_FLOAT_EQ(local_matrix[3][2], 3.0f, 1e-5f); // z-компонента позиции
    
    // Проверим преобразование точки
    Vector3 point(1.0f, 0.0f, 0.0f);
    Vector3 transformed = t.local_to_world(point);
    
    // С учетом позиции (1,2,3) и масштаба 2, точка (1,0,0) станет (3, 2, 3)
    ASSERT_FLOAT_EQ(transformed.x(), 3.0f, 1e-5f);
    ASSERT_FLOAT_EQ(transformed.y(), 2.0f, 1e-5f);
    ASSERT_FLOAT_EQ(transformed.z(), 3.0f, 1e-5f);
    
    std::cout << "Матрицы трансформации протестированы!" << std::endl;
    return true;
}

// Тестирование методов look_at
bool test_transform_look_at() {
    std::cout << "Тестирование метода look_at..." << std::endl;
    
    Transform t;
    t.set_position(0.0f, 0.0f, 0.0f);
    Vector3 target(0.0f, 0.0f, 1.0f);
    Vector3 up(0.0f, 1.0f, 0.0f);
    
    t.look_at(target, up);
    
    // После look_at вектор направления должен быть (0, 0, 1)
    const Matrix4x4& matrix = t.get_world_matrix();
    Vector3 forward(matrix[2][0], matrix[2][1], matrix[2][2]); // 3-я строка - направление Z
    forward = forward * -1; // Инвертируем, т.к. в OpenGL Z идет вглубь сцены
    
    // Проверим, что вектор направления близок к (0, 0, 1)
    ASSERT_FLOAT_EQ(forward.x(), 0.0f, 0.1f);
    ASSERT_FLOAT_EQ(forward.y(), 0.0f, 0.1f);
    ASSERT_FLOAT_EQ(forward.z(), 1.0f, 0.1f);
    
    std::cout << "Метод look_at протестирован!" << std::endl;
    return true;
}

// Тестирование преобразований
bool test_transform_operations() {
    std::cout << "Тестирование операций Transform..." << std::endl;
    
    Transform t;
    t.set_position(1.0f, 1.0f, 1.0f);
    
    // Тестирование перемещения
    t.translate(1.0f, 0.0f, 0.0f);
    ASSERT_FLOAT_EQ(t.get_position().x(), 2.0f, 1e-5f);
    
    // Тестирование вращения
    t.rotate(45.0f, 0.0f, 0.0f);
    ASSERT_FLOAT_EQ(t.get_rotation().x(), 45.0f, 1e-5f);
    
    // Тестирование масштабирования
    t.scale(2.0f, 1.0f, 1.0f);
    ASSERT_FLOAT_EQ(t.get_scale().x(), 2.0f, 1e-5f);
    
    std::cout << "Операции Transform протестированы!" << std::endl;
    return true;
}

int main() {
    std::cout << "=== Запуск тестов для класса Transform ===" << std::endl;
    
    bool all_passed = true;
    
    all_passed &= test_transform_basic();
    all_passed &= test_transform_hierarchy();
    all_passed &= test_transform_matrices();
    all_passed &= test_transform_look_at();
    all_passed &= test_transform_operations();
    
    if(all_passed) {
        std::cout << "\n=== Все тесты Transform пройдены успешно! ===" << std::endl;
    } else {
        std::cout << "\n=== Некоторые тесты Transform провалены! ===" << std::endl;
        return 1;
    }
    
    return 0;
}