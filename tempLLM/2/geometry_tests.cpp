#include "math_lib.hpp"
#include <iostream>
#include <cmath>

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

// Константа для погрешности
const real EPSILON = 1e-5f;

// Тесты для геометрических примитивов
bool test_primitives() {
    std::cout << "Тестирование геометрических примитивов..." << std::endl;
    
    // Тест плоскости
    Plane plane1(Vector3(0, 1, 0), 5.0f); // Горизонтальная плоскость на высоте 5
    ASSERT_FLOAT_EQ(plane1.distance_to_point(Vector3(0, 5, 0)), 0.0f, EPSILON);
    ASSERT_FLOAT_EQ(plane1.distance_to_point(Vector3(0, 6, 0)), 1.0f, EPSILON);
    ASSERT_FLOAT_EQ(plane1.distance_to_point(Vector3(0, 4, 0)), -1.0f, EPSILON);
    
    // Создание плоскости по трем точкам
    Vector3 p1(0, 0, 0);
    Vector3 p2(1, 0, 0);
    Vector3 p3(0, 1, 0);
    Plane plane2 = Plane::from_points(p1, p2, p3);
    ASSERT_FLOAT_EQ(plane2.normal.x(), 0.0f, EPSILON);
    ASSERT_FLOAT_EQ(plane2.normal.y(), 0.0f, EPSILON);
    ASSERT_FLOAT_EQ(plane2.normal.z(), 1.0f, EPSILON);
    ASSERT_FLOAT_EQ(plane2.distance, 0.0f, EPSILON);
    
    // Тест сферы
    Sphere sphere1(Vector3(0, 0, 0), 2.0f);
    ASSERT(sphere1.contains_point(Vector3(0, 0, 0)), "Сфера должна содержать центр");
    ASSERT(sphere1.contains_point(Vector3(1, 1, 1)), "Сфера должна содержать точку внутри");
    ASSERT(!sphere1.contains_point(Vector3(3, 0, 0)), "Сфера не должна содержать точку вне");
    
    // Проверка пересечения сфер
    Sphere sphere2(Vector3(3, 0, 0), 2.0f);
    ASSERT(sphere1.intersects_sphere(sphere2), "Сферы должны пересекаться");
    Sphere sphere3(Vector3(5, 0, 0), 1.0f);
    ASSERT(!sphere1.intersects_sphere(sphere3), "Сферы не должны пересекаться");
    
    // Объем и площадь поверхности сферы
    ASSERT_FLOAT_EQ(sphere1.volume(), (4.0f/3.0f) * math_constants::PI * 8.0f, 1e-3f);
    ASSERT_FLOAT_EQ(sphere1.surface_area(), 4.0f * math_constants::PI * 4.0f, 1e-3f);
    
    // Тест AABB
    AABB aabb1(Vector3(-1, -1, -1), Vector3(1, 1, 1));
    ASSERT(aabb1.contains_point(Vector3(0, 0, 0)), "AABB должен содержать центр");
    ASSERT(aabb1.contains_point(Vector3(0.5f, 0.5f, 0.5f)), "AABB должен содержать внутреннюю точку");
    ASSERT(!aabb1.contains_point(Vector3(2, 0, 0)), "AABB не должен содержать внешнюю точку");
    
    // Проверка пересечения AABB
    AABB aabb2(Vector3(0, 0, 0), Vector3(2, 2, 2));
    ASSERT(aabb1.intersects_aabb(aabb2), "AABB должны пересекаться");
    AABB aabb3(Vector3(3, 3, 3), Vector3(4, 4, 4));
    ASSERT(!aabb1.intersects_aabb(aabb3), "AABB не должны пересекаться");
    
    // Объем и площадь поверхности AABB
    ASSERT_FLOAT_EQ(aabb1.volume(), 8.0f, EPSILON);
    ASSERT_FLOAT_EQ(aabb1.surface_area(), 24.0f, EPSILON);
    
    // Центр и размеры AABB
    Vector3 center = aabb1.center();
    ASSERT_FLOAT_EQ(center.x(), 0.0f, EPSILON);
    ASSERT_FLOAT_EQ(center.y(), 0.0f, EPSILON);
    ASSERT_FLOAT_EQ(center.z(), 0.0f, EPSILON);
    
    Vector3 size = aabb1.size();
    ASSERT_FLOAT_EQ(size.x(), 2.0f, EPSILON);
    ASSERT_FLOAT_EQ(size.y(), 2.0f, EPSILON);
    ASSERT_FLOAT_EQ(size.z(), 2.0f, EPSILON);
    
    // Тест OBB
    OBB obb1;
    ASSERT_FLOAT_EQ(obb1.volume(), 8.0f * 0.5f * 0.5f * 0.5f, EPSILON);
    
    // Создание OBB из AABB
    OBB obb2 = OBB::from_aabb(aabb1);
    Vector3 obb2_center = obb2.center;
    ASSERT_FLOAT_EQ(obb2_center.x(), center.x(), EPSILON);
    ASSERT_FLOAT_EQ(obb2_center.y(), center.y(), EPSILON);
    ASSERT_FLOAT_EQ(obb2_center.z(), center.z(), EPSILON);
    
    // Тест капсулы
    Capsule capsule1(Vector3(0, -1, 0), Vector3(0, 1, 0), 0.5f);
    ASSERT(capsule1.contains_point(Vector3(0, 0, 0)), "Капсула должна содержать центр");
    ASSERT(capsule1.contains_point(Vector3(0, 0, 0.4f)), "Капсула должна содержать точку внутри");
    ASSERT(!capsule1.contains_point(Vector3(0, 0, 1.0f)), "Капсула не должна содержать точку вне");
    
    // Длина осевого отрезка капсулы
    ASSERT_FLOAT_EQ(capsule1.axis_length(), 2.0f, EPSILON);
    
    // Объем и площадь поверхности капсулы
    real expected_volume = math_constants::PI * 0.5f * 0.5f * 2.0f + (4.0f/3.0f) * math_constants::PI * 0.5f * 0.5f * 0.5f;
    real expected_surface = 2.0f * math_constants::PI * 0.5f * 2.0f + 4.0f * math_constants::PI * 0.5f * 0.5f;
    ASSERT_FLOAT_EQ(capsule1.volume(), expected_volume, 1e-3f);
    ASSERT_FLOAT_EQ(capsule1.surface_area(), expected_surface, 1e-3f);
    
    // Тест конуса
    Cone cone1(Vector3(0, 0, 0), Vector3(0, 1, 0), 2.0f, 1.0f); // Основание в (0,0,0), высота 2, радиус 1
    ASSERT(cone1.contains_point(Vector3(0, 0.5f, 0)), "Конус должен содержать точку внутри");
    ASSERT(!cone1.contains_point(Vector3(2, 0.5f, 0)), "Конус не должен содержать точку вне");
    
    // Высота конуса
    ASSERT_FLOAT_EQ(cone1.height(), 2.0f, EPSILON);
    
    // Объем и площадь поверхности конуса
    real expected_cone_volume = (1.0f/3.0f) * math_constants::PI * 1.0f * 1.0f * 2.0f;
    real expected_cone_surface = math_constants::PI * 1.0f * (1.0f + std::sqrt(1.0f + 4.0f)); // r * (r + sqrt(r² + h²))
    ASSERT_FLOAT_EQ(cone1.volume(), expected_cone_volume, 1e-3f);
    ASSERT_FLOAT_EQ(cone1.surface_area(), expected_cone_surface, 1e-3f);
    
    // Тест фрустума
    Matrix4x4 proj = Matrix4x4::perspective_lh(math_constants::PI / 2.0f, 1.0f, 0.1f, 100.0f);
    Matrix4x4 view = Matrix4x4::look_at_lh(Vector3(0, 0, 5), Vector3(0, 0, 0), Vector3(0, 1, 0));
    Matrix4x4 view_proj = proj * view;
    Frustum frustum = Frustum::from_matrix(view_proj);
    
    // Тесты содержания точек
    ASSERT(frustum.contains_point(Vector3(0, 0, 50)), "Точка должна быть внутри фрустума");
    ASSERT(!frustum.contains_point(Vector3(0, 0, -50)), "Точка не должна быть внутри фрустума");
    
    // Тесты пересечения с геометрическими примитивами
    ASSERT(frustum.intersects_aabb(AABB(Vector3(-0.5f, -0.5f, 49), Vector3(0.5f, 0.5f, 51))), "AABB должен пересекать фрустум");
    ASSERT(frustum.intersects_sphere(Sphere(Vector3(0, 0, 50), 1.0f)), "Сфера должна пересекать фрустум");
    
    std::cout << "Геометрические примитивы: OK" << std::endl;
    return true;
}

// Тесты для функций проверки столкновений
bool test_collision_functions() {
    std::cout << "Тестирование функций проверки столкновений..." << std::endl;
    
    // Тест пересечения луча со сферой
    Sphere sphere(Vector3(0, 0, 0), 1.0f);
    Vector3 ray_origin(0, 0, -5);
    Vector3 ray_direction(0, 0, 1);
    
    real t1, t2;
    bool intersects = sphere.ray_intersect(ray_origin, ray_direction, t1, t2);
    ASSERT(intersects, "Луч должен пересекать сферу");
    ASSERT_FLOAT_EQ(t1, 4.0f, 1e-4f); // 5 - 1 = 4
    ASSERT_FLOAT_EQ(t2, 6.0f, 1e-4f); // 5 + 1 = 6
    
    // Тест пересечения луча с AABB
    AABB aabb(Vector3(-1, -1, -1), Vector3(1, 1, 1));
    intersects = collision::ray_aabb(ray_origin, ray_direction, aabb).collided;
    ASSERT(intersects, "Луч должен пересекать AABB");
    
    // Тест пересечения луча с плоскостью
    Plane plane(Vector3(0, 0, 1), 0.0f); // Z=0 плоскость
    intersects = collision::ray_plane(ray_origin, ray_direction, plane).collided;
    ASSERT(intersects, "Луч должен пересекать плоскость");
    
    // Тесты столкновений между примитивами
    // Сфера-сфера
    Sphere sphere2(Vector3(1.5f, 0, 0), 1.0f);
    ASSERT(collision::sphere_sphere(sphere, sphere2).collided, "Сферы должны пересекаться");
    Sphere sphere3(Vector3(3, 0, 0), 0.5f);
    ASSERT(!collision::sphere_sphere(sphere, sphere3).collided, "Сферы не должны пересекаться");
    
    // Сфера-AABB
    AABB aabb2(Vector3(0.5f, 0.5f, 0.5f), Vector3(1.5f, 1.5f, 1.5f));
    ASSERT(collision::sphere_aabb(sphere, aabb2).collided, "Сфера и AABB должны пересекаться");
    AABB aabb3(Vector3(5, 5, 5), Vector3(6, 6, 6));
    ASSERT(!collision::sphere_aabb(sphere, aabb3).collided, "Сфера и AABB не должны пересекаться");
    
    std::cout << "Функции проверки столкновений: OK" << std::endl;
    return true;
}

// Тесты для функций из пространства имен collision
bool test_collision_namespace() {
    std::cout << "Тестирование функций из пространства имен collision..." << std::endl;
    
    // Тест point_sphere
    Sphere sphere(Vector3(0, 0, 0), 1.0f);
    CollisionInfo info = collision::point_sphere(Vector3(0.5f, 0.5f, 0.5f), sphere);
    ASSERT(info.collided, "Точка должна находиться внутри сферы");
    ASSERT(info.penetration_depth > 0.0f, "Должна быть положительная глубина проникновения");
    
    // Тест point_aabb
    AABB aabb(Vector3(-1, -1, -1), Vector3(1, 1, 1));
    info = collision::point_aabb(Vector3(0.5f, 0.5f, 0.5f), aabb);
    ASSERT(info.collided, "Точка должна находиться внутри AABB");
    
    // Тест sphere_aabb
    info = collision::sphere_aabb(sphere, aabb);
    ASSERT(info.collided, "Сфера и AABB должны пересекаться");
    
    // Тест ray_sphere
    Vector3 ray_origin(0, 0, -5);
    Vector3 ray_direction(0, 0, 1);
    info = collision::ray_sphere(ray_origin, ray_direction, sphere);
    ASSERT(info.collided, "Луч должен пересекать сферу");
    ASSERT_FLOAT_EQ(info.collision_time, 4.0f, 1e-4f);
    
    // Тест ray_aabb
    info = collision::ray_aabb(ray_origin, ray_direction, aabb);
    ASSERT(info.collided, "Луч должен пересекать AABB");
    ASSERT_FLOAT_EQ(info.collision_time, 4.0f, 1e-4f);
    
    // Тест ray_plane
    Plane plane(Vector3(0, 0, 1), 0.0f);
    info = collision::ray_plane(ray_origin, ray_direction, plane);
    ASSERT(info.collided, "Луч должен пересекать плоскость");
    ASSERT_FLOAT_EQ(info.collision_time, 5.0f, 1e-4f);
    
    std::cout << "Функции из пространства имен collision: OK" << std::endl;
    return true;
}

int main() {
    std::cout << "=== Запуск тестов геометрических примитивов и столкновений ===" << std::endl;
    
    bool all_tests_passed = true;
    
    all_tests_passed &= test_primitives();
    all_tests_passed &= test_collision_functions();
    all_tests_passed &= test_collision_namespace();
    
    if(all_tests_passed) {
        std::cout << "\n=== Все тесты геометрических примитивов пройдены успешно! ===" << std::endl;
        return 0;
    } else {
        std::cout << "\n=== Некоторые тесты геометрических примитивов провалены! ===" << std::endl;
        return 1;
    }
}