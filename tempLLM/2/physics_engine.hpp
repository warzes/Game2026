#pragma once

#include "math_lib.hpp"
#include <vector>
#include <limits>

// Структура для представления треугольника в мире
struct Triangle {
    Vector3 vertices[3];
    
    constexpr Triangle() = default;
    constexpr Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
    }
    
    // Получение плоскости треугольника
    constexpr Plane get_plane() const {
        return Plane::from_points(vertices[0], vertices[1], vertices[2]);
    }
    
    // Получение нормали треугольника
    constexpr Vector3 get_normal() const {
        Vector3 edge1 = vertices[1] - vertices[0];
        Vector3 edge2 = vertices[2] - vertices[0];
        return edge1.cross(edge2).normalized();
    }
};

// Структура для информации о столкновении с треугольником
struct TriangleCollisionInfo {
    bool collided = false;
    Vector3 contact_point;
    Vector3 contact_normal;
    real penetration_depth = 0.0f;
    real collision_time = 0.0f;
    int triangle_index = -1; // Индекс треугольника в сетке
};

// Класс для представления сетки из треугольников
class TriangleMesh {
private:
    std::vector<Triangle> triangles;
    AABB bounding_box; // Ограничивающий объем для оптимизации
    
public:
    TriangleMesh() = default;
    
    // Добавление треугольника в сетку
    void add_triangle(const Triangle& tri) {
        triangles.push_back(tri);
        
        // Обновление ограничивающего объема
        if (triangles.size() == 1) {
            bounding_box = AABB(tri.vertices[0], tri.vertices[0]);
            for (int i = 1; i < 3; i++) {
                bounding_box = bounding_box.expanded_to_include(tri.vertices[i]);
            }
        } else {
            for (int i = 0; i < 3; i++) {
                bounding_box = bounding_box.expanded_to_include(tri.vertices[i]);
            }
        }
    }
    
    // Добавление нескольких треугольников
    void add_triangles(const std::vector<Triangle>& tris) {
        for (const auto& tri : tris) {
            add_triangle(tri);
        }
    }
    
    // Получение количества треугольников
    size_t get_triangle_count() const {
        return triangles.size();
    }
    
    // Получение треугольника по индексу
    const Triangle& get_triangle(size_t index) const {
        return triangles[index];
    }
    
    // Получение ограничивающего объема
    const AABB& get_bounding_box() const {
        return bounding_box;
    }
    
    // Проверка пересечения сферы с сеткой
    std::vector<TriangleCollisionInfo> sphere_collisions(const Sphere& sphere) const {
        std::vector<TriangleCollisionInfo> collisions;
        
        // Сначала проверяем, пересекается ли сфера с ограничивающим объемом
        if (!bounding_box.intersects_aabb(AABB(sphere.center - Vector3(sphere.radius, sphere.radius, sphere.radius),
                                              sphere.center + Vector3(sphere.radius, sphere.radius, sphere.radius)))) {
            return collisions;
        }
        
        // Проверяем каждый треугольник
        for (size_t i = 0; i < triangles.size(); i++) {
            const Triangle& tri = triangles[i];
            
            // Проверяем, пересекается ли сфера с плоскостью треугольника
            Plane plane = tri.get_plane();
            real dist_to_plane = plane.distance_to_point(sphere.center);
            
            if (std::abs(dist_to_plane) <= sphere.radius) {
                // Проверяем, находится ли центр сферы близко к треугольнику
                // Используем алгоритм ближайшей точки на треугольнике
                
                Vector3 closest_point = closest_point_on_triangle(sphere.center, tri);
                Vector3 diff = sphere.center - closest_point;
                
                if (diff.length_squared() <= sphere.radius * sphere.radius) {
                    TriangleCollisionInfo info;
                    info.collided = true;
                    info.contact_point = closest_point;
                    info.contact_normal = diff.normalized();
                    info.penetration_depth = sphere.radius - diff.length();
                    info.triangle_index = static_cast<int>(i);
                    
                    collisions.push_back(info);
                }
            }
        }
        
        return collisions;
    }
    
private:
    // Нахождение ближайшей точки на треугольнике к заданной точке
    Vector3 closest_point_on_triangle(const Vector3& point, const Triangle& tri) const {
        Vector3 a = tri.vertices[0];
        Vector3 b = tri.vertices[1];
        Vector3 c = tri.vertices[2];
        
        // Проверяем, является ли точка внутри треугольника
        Vector3 ab = b - a;
        Vector3 ac = c - a;
        Vector3 ap = point - a;
        
        real d1 = ab.dot(ap);
        real d2 = ac.dot(ap);
        
        if (d1 <= 0.0f && d2 <= 0.0f) {
            // Ближайшая точка - вершина a
            return a;
        }
        
        Vector3 bp = point - b;
        real d3 = ab.dot(bp);
        real d4 = ac.dot(bp);
        
        if (d3 >= 0.0f && d4 <= d3) {
            // Ближайшая точка - вершина b
            return b;
        }
        
        real vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            // Ближайшая точка на ребре ab
            real v = d1 / (d1 - d3);
            return a + ab * v;
        }
        
        Vector3 cp = point - c;
        real d5 = ab.dot(cp);
        real d6 = ac.dot(cp);
        
        if (d6 >= 0.0f && d5 <= d6) {
            // Ближайшая точка - вершина c
            return c;
        }
        
        real vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            // Ближайшая точка на ребре ac
            real w = d2 / (d2 - d6);
            return a + ac * w;
        }
        
        real va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            // Ближайшая точка на ребре bc
            real w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + (c - b) * w;
        }
        
        // Точка внутри треугольника
        real denom = 1.0f / (va + vb + vc);
        real v = vb * denom;
        real w = vc * denom;
        return a + ab * v + ac * w;
    }
};

// Класс для физического тела (игрока)
class PhysicsBody {
public:
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    real mass;
    real radius; // Радиус для коллизий (используем сферу)
    real friction;
    real restitution; // Коэффициент упругости
    bool on_ground;
    
    // Конструктор
    PhysicsBody(const Vector3& pos = Vector3(0, 0, 0), real r = 0.5f)
        : position(pos), velocity(0, 0, 0), acceleration(0, 0, 0), 
          mass(1.0f), radius(r), friction(0.8f), restitution(0.1f), on_ground(false) {}
    
    // Обновление физики
    void update(real delta_time, const Vector3& gravity = Vector3(0, -9.81f, 0)) {
        // Применение гравитации
        acceleration = gravity;
        
        // Интегрирование движения (метод Эйлера)
        velocity = velocity + acceleration * delta_time;
        Vector3 displacement = velocity * delta_time;
        
        // Применение трения (только горизонтальное)
        if (on_ground) {
            Vector3 horizontal_velocity = Vector3(velocity.x(), 0, velocity.z());
            horizontal_velocity = horizontal_velocity * friction;
            velocity = Vector3(horizontal_velocity.x(), velocity.y(), horizontal_velocity.z());
        }
        
        position = position + displacement;
    }
};

// Класс физического движка
class PhysicsEngine {
private:
    TriangleMesh world_mesh;
    Vector3 gravity;
    real ground_friction;
    
public:
    // Конструктор
    PhysicsEngine(const Vector3& g = Vector3(0, -9.81f, 0), real friction = 0.8f)
        : gravity(g), ground_friction(friction) {}
    
    // Добавление треугольника в мир
    void add_triangle(const Triangle& tri) {
        world_mesh.add_triangle(tri);
    }
    
    // Добавление нескольких треугольников
    void add_triangles(const std::vector<Triangle>& tris) {
        for (const auto& tri : tris) {
            add_triangle(tri);
        }
    }
    
    // Обновление физики тела с учетом столкновений
    void update_body(PhysicsBody& body, real delta_time) {
        // Сохраняем старую позицию для расчета скольжения
        Vector3 old_position = body.position;
        
        // Применение гравитации
        body.acceleration = gravity;
        
        // Интегрирование движения
        body.velocity = body.velocity + body.acceleration * delta_time;
        Vector3 displacement = body.velocity * delta_time;
        
        // Применение трения
        if (body.on_ground) {
            Vector3 horizontal_velocity = Vector3(body.velocity.x(), 0, body.velocity.z());
            horizontal_velocity = horizontal_velocity * body.friction;
            body.velocity = Vector3(horizontal_velocity.x(), body.velocity.y(), horizontal_velocity.z());
        }
        
        // Перемещение с проверкой столкновений
        body.position = body.position + displacement;
        resolve_collisions(body);
    }
    
    // Разрешение столкновений для тела
    void resolve_collisions(PhysicsBody& body) {
        // Создаем сферу для проверки столкновений
        Sphere sphere(body.position, body.radius);
        
        // Получаем список столкновений
        auto collisions = world_mesh.sphere_collisions(sphere);
        
        // Если нет столкновений, просто выходим
        if (collisions.empty()) {
            body.on_ground = false;
            return;
        }
        
        // Обрабатываем каждое столкновение
        for (const auto& collision : collisions) {
            if (collision.collided) {
                // Корректируем позицию тела
                Vector3 correction = collision.contact_normal * collision.penetration_depth;
                body.position = body.position + correction;
                
                // Обновляем сферу после коррекции
                sphere.center = body.position;
                
                // Определяем, находится ли тело на земле
                if (std::abs(collision.contact_normal.y()) > 0.7f) {
                    body.on_ground = true;
                }
                
                // Обработка отскока и скольжения
                handle_collision_response(body, collision);
            }
        }
    }
    
    // Обработка ответа на столкновение
    void handle_collision_response(PhysicsBody& body, const TriangleCollisionInfo& collision) {
        // Разделяем скорость на нормальную и тангенциальную составляющие
        real normal_speed = body.velocity.dot(collision.contact_normal);
        
        if (normal_speed < 0) {
            // Тело движется в сторону поверхности
            Vector3 normal_component = collision.contact_normal * normal_speed;
            Vector3 tangent_component = body.velocity - normal_component;
            
            // Применяем упругость к нормальной составляющей
            normal_component = normal_component * (-body.restitution);
            
            // Применяем трение к тангенциальной составляющей
            tangent_component = tangent_component * body.friction;
            
            // Новая скорость после столкновения
            body.velocity = tangent_component + normal_component;
            
            // Если нормальная скорость мала, считаем, что тело на земле
            if (std::abs(normal_speed) < 0.1f && std::abs(collision.contact_normal.y()) > 0.7f) {
                body.on_ground = true;
                // Убираем вертикальную скорость для предотвращения "липкости"
                body.velocity = Vector3(body.velocity.x(), 0, body.velocity.z());
            }
        }
    }
    
    // Проверка, находится ли тело на земле
    bool is_body_on_ground(const PhysicsBody& body) const {
        Sphere sphere(body.position, body.radius);
        auto collisions = world_mesh.sphere_collisions(sphere);
        
        for (const auto& collision : collisions) {
            if (collision.collided && std::abs(collision.contact_normal.y()) > 0.7f) {
                return true;
            }
        }
        return false;
    }
    
    // Прыжок тела
    void jump_body(PhysicsBody& body, real jump_force = 5.0f) {
        if (body.on_ground) {
            body.velocity = body.velocity + Vector3(0, jump_force, 0);
            body.on_ground = false;
        }
    }
    
    // Перемещение тела с обработкой скольжения
    void move_body(PhysicsBody& body, const Vector3& movement, real delta_time) {
        // Сохраняем начальную позицию
        Vector3 initial_position = body.position;
        
        // Попытка перемещения
        body.position = body.position + movement * delta_time;
        
        // Проверка и разрешение столкновений
        resolve_collisions_with_sliding(body, initial_position);
    }
    
    // Разрешение столкновений с учетом скольжения
    void resolve_collisions_with_sliding(PhysicsBody& body, const Vector3& initial_position) {
        // Проверяем столкновения
        Sphere sphere(body.position, body.radius);
        auto collisions = world_mesh.sphere_collisions(sphere);
        
        if (collisions.empty()) {
            return;
        }
        
        // Обрабатываем столкновения с применением скольжения
        for (const auto& collision : collisions) {
            if (collision.collided) {
                // Вычисляем вектор от начальной точки до точки столкновения
                Vector3 slide_direction = body.position - initial_position;
                
                // Проекция направления скольжения на плоскость столкновения
                Vector3 tangent_to_normal = slide_direction - collision.contact_normal * slide_direction.dot(collision.contact_normal);
                
                // Корректируем позицию тела
                Vector3 correction = collision.contact_normal * collision.penetration_depth;
                body.position = body.position + correction;
                
                // Применяем скольжение: уменьшаем скорость в направлении нормали
                real normal_speed = body.velocity.dot(collision.contact_normal);
                if (normal_speed < 0) {
                    body.velocity = body.velocity - collision.contact_normal * normal_speed * (1.0f + body.restitution);
                }
                
                // Если угол наклона достаточно крутой, тело может скользить вниз
                if (collision.contact_normal.y() < 0.7f && collision.contact_normal.y() > 0.0f) {
                    // Добавляем компоненту силы тяжести вдоль наклонной поверхности
                    Vector3 gravity_component = gravity - collision.contact_normal * gravity.dot(collision.contact_normal);
                    body.velocity = body.velocity + gravity_component;
                }
            }
        }
    }
};

// Класс для управления перемещением игрока
class PlayerController {
private:
    PhysicsBody body;
    real movement_speed;
    real jump_force;
    real max_slope_angle; // Максимальный угол наклона, по которому можно подниматься
    
public:
    PlayerController(const Vector3& position = Vector3(0, 0, 0), real player_radius = 0.5f)
        : body(position, player_radius), movement_speed(5.0f), jump_force(8.0f), max_slope_angle(45.0f) {}
    
    // Обновление контроллера
    void update(PhysicsEngine& engine, real delta_time,
                bool forward, bool backward, bool left, bool right, bool jump,
                const Vector3& view_direction) {
        
        // Вычисление направления движения
        Vector3 movement(0, 0, 0);
        
        if (forward) movement = movement + view_direction;
        if (backward) movement = movement - view_direction;
        
        // Боковое движение (перпендикулярно направлению взгляда)
        Vector3 strafe_direction = view_direction.cross(Vector3(0, 1, 0)).normalized();
        if (left) movement = movement - strafe_direction;
        if (right) movement = movement + strafe_direction;
        
        // Нормализация и масштабирование
        if (movement.length_squared() > 0) {
            movement = movement.normalized() * movement_speed;
        }
        
        // Применение движения
        if (body.on_ground) {
            // На земле - только горизонтальное движение
            body.velocity = Vector3(movement.x(), body.velocity.y(), movement.z());
        } else {
            // В воздухе - можно двигаться по всем осям, но с меньшей скоростью
            body.velocity = Vector3(movement.x() * 0.5f, body.velocity.y(), movement.z() * 0.5f);
        }
        
        // Прыжок
        if (jump && body.on_ground) {
            body.velocity = body.velocity + Vector3(0, jump_force, 0);
            body.on_ground = false;
        }
        
        // Обновление физики
        engine.update_body(body, delta_time);
    }
    
    // Получение позиции игрока
    const Vector3& get_position() const {
        return body.position;
    }
    
    // Установка позиции игрока
    void set_position(const Vector3& pos) {
        body.position = pos;
    }
    
    // Получение скорости игрока
    const Vector3& get_velocity() const {
        return body.velocity;
    }
    
    // Проверка, находится ли игрок на земле
    bool is_on_ground() const {
        return body.on_ground;
    }
    
    // Прыжок
    void jump() {
        if (body.on_ground) {
            body.velocity = body.velocity + Vector3(0, jump_force, 0);
            body.on_ground = false;
        }
    }
    
    // Перемещение
    void move(const Vector3& direction, real delta_time) {
        Vector3 movement = direction.normalized() * movement_speed;
        body.velocity = Vector3(movement.x(), body.on_ground ? 0 : body.velocity.y(), movement.z());
    }
};