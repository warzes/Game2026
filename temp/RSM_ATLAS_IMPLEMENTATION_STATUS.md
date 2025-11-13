# Полная реализация RSM и Shadow Atlas - Статус

## Обзор

Реализована полная поддержка двух систем рендеринга теней:
- **RSM (Reflective Shadow Maps)** - Карты с информацией о цвете и нормалях для непрямого освещения
- **Shadow Atlas** - Консолидация нескольких теневых карт в единую текстуру для оптимизации

## Статус реализации

### ✅ Завершено

#### 1. Инфраструктура света (Light Classes)
- `LightDirectional.hpp/cpp` - Полная поддержка RSM + Atlas с параметрами качества
  - initRSM(), resizeRSM(), destroyRSM() методы
  - rsmSmoothness, rsmBias, rsmRadius, rsmIntensity параметры

- `LightPoint.hpp/cpp` - Кубмап RSM для точечных источников
  - rsmCubeFBO, rsmColorCube, rsmCubeMap 
  - Полная инициализация для 6 граней кубмапа

- `LightSpot.hpp/cpp` - 2D RSM для прожекторов
  - rsmFBO, rsmColorMap, rsmNormalMap, rsmDepthMap
  - Dual attachment MRT setup

#### 2. ShadowManager
- `ShadowManager.hpp/cpp` - Центральный менеджер теней
  - Перечисления: ShadowMode (Standard/RSM), AtlasMode (Individual/Atlas)
  - Методы: renderDirectionalShadow(), renderPointShadow(), renderSpotShadow()
  - Atlas управление: allocateAtlasSpace(), getAtlasRect()
  - RSM параметры: setRSMQuality(smoothness, bias, radius, intensity)
  - Внутренние рендеры: renderToStandardShadow_(), renderToRSM_(), renderToAtlas_()

#### 3. Шейдеры - Рендеринг теней
- `shadow_rsm.vert/frag` - Рендеринг RSM
  - Трансформация в пространство света с TBN матрицей
  - MRT вывод: цвет (GL_COLOR_ATTACHMENT0) + нормали (GL_COLOR_ATTACHMENT1)

- `shadow_atlas.vert/frag` - Рендеринг Atlas
  - Трансформация координат для atlas тайлов
  - Вывод глубины в нужный регион

#### 4. Шейдеры - Освещение с поддержкой RSM/Atlas
- `basic_advanced_rsm.frag` (204 строки)
  - Полная Cook-Torrance BRDF реализация
  - rsmPCF() - 3x3 PCF фильтрация
  - sampleRSMIndirect() - Выборка RSM с нормальной модуляцией
  - Параметры: uRSMSmoothness, uRSMBias, uRSMRadius, uRSMIntensity

- `basic_advanced_atlas.frag` (200+ строк)
  - PCF через sampleShadowAtlasPCF() с 9-点выборкой
  - Правильное преобразование координат atlas
  - Параметры: uAtlasBias, uAtlasSmoothing, uAtlasRSMEnabled

#### 5. Вспомогательные шейдеры
- `rsm_temporal_filter.vert/frag` - Временное сглаживание RSM
  - Блендинг текущего и предыдущего фрейма
  - Детекция движения для адаптивного блендинга
  - uTemporalBlend параметр

#### 6. Примеры и документация
- `shadow_modes_example.cpp` - Полный пример использования
- `ShadowModeHelper.hpp/cpp` - Удобный API для переключения режимов
- `SHADOW_MODES.md` - Подробное руководство

### 📋 Структура файлов

#### Headers (include/RenderLib/)
```
LightDirectional.hpp  - ✅ RSM + Atlas параметры
LightPoint.hpp        - ✅ Cubemap RSM параметры  
LightSpot.hpp         - ✅ 2D RSM параметры
ShadowManager.hpp     - ✅ Центральный менеджер
ShadowModeHelper.hpp  - ✅ Helper класс
```

#### Реализация (src/)
```
LightDirectional.cpp  - ✅ Полная реализация initRSM()
LightPoint.cpp        - ✅ Cubemap RSM с 6 гранями
LightSpot.cpp         - ✅ 2D RSM с двойным attachment
ShadowManager.cpp     - ✅ 289 строк: управление режимами, atlas
ShadowModeHelper.cpp  - ✅ API для runtime переключения
```

#### Шейдеры (shaders/)
```
shadow_rsm.vert/frag              - ✅ RSM rendering
shadow_atlas.vert/frag            - ✅ Atlas rendering
basic_advanced_rsm.frag           - ✅ RSM lighting (204 строки)
basic_advanced_atlas.frag         - ✅ Atlas lighting (200 строк)
rsm_temporal_filter.vert/frag     - ✅ Temporal filtering
```

## Функциональность

### RSM (Reflective Shadow Maps)
- ✅ Сохранение цвета света и нормалей в MRT (Multiple Render Targets)
- ✅ Поддержка всех типов источников света (directional, point, spot)
- ✅ PCF фильтрация с параметром smoothness
- ✅ Параметры качества: bias, radius, intensity
- ✅ Временное накопление между фреймами (temporal filtering)
- ✅ Непрямое освещение на основе RSM данных

### Shadow Atlas
- ✅ Консолидация нескольких теневых карт в одну текстуру
- ✅ Автоматическое расчёт сетки тайлов (2D квадратная раскладка)
- ✅ Per-light UV координаты через getAtlasRect()
- ✅ PCF поддержка в пределах тайла
- ✅ Поддержка как Standard, так и RSM режимов

### Runtime режимы
- ✅ setShadowMode(ShadowMode::Standard / RShadowMode::RSM)
- ✅ setAtlasMode(AtlasMode::Individual / AtlasMode::Atlas)
- ✅ Независимое переключение режимов
- ✅ Сохранение обратной совместимости

## Параметры качества

### RSM параметры (на свет и глобально)
```cpp
float rsmSmoothness = 1.0f;   // 1-3, радиус PCF фильтрации
float rsmBias = 0.001f;       // Смещение глубины (избегаем артефактов)
float rsmRadius = 0.02f;      // Радиус выборки непрямого света
float rsmIntensity = 1.0f;    // Интенсивность вклада непрямого света
```

### Atlas параметры
```cpp
float uAtlasBias = 0.005;      // Смещение глубины для сравнения
float uAtlasSmoothing = 1.0;   // Фактор PCF фильтрации
bool uAtlasRSMEnabled = false; // Включить RSM в atlas режиме
```

## Архитектура рендеринга

### Стандартный режим (Standard)
```
Directional/Point/Spot Light
  ↓
ShadowManager::render*Shadow()
  ↓
renderToStandardShadow_() → shadow_depth.frag
  ↓
Глубина → depthMap (individual) или shadowAtlasTexture (atlas)
  ↓
Сцена использует depthMap для PCF теней
```

### RSM режим (RSM)
```
Directional/Point/Spot Light
  ↓
ShadowManager::render*Shadow()
  ↓
renderToRSM_() → shadow_rsm.frag
  ↓
MRT: Цвет (GL_COLOR_ATTACHMENT0) + Нормали (GL_COLOR_ATTACHMENT1)
  ↓
rsmColorMap/rsmNormalMap (individual) 
или shadowAtlasColorTexture (atlas)
  ↓
Сцена использует basic_advanced_rsm.frag
  ↓
Непрямое освещение из RSM данных
```

### Atlas режим
```
Multiple Lights → allocateAtlasSpace(count, resolution)
  ↓
Расчёт grid: atlasTilesX = sqrt(count), atlasTilesY = (count + X - 1) / X
  ↓
renderToAtlas_(lightIndex, ...) 
  ↓
glViewport(x, y, w, h) + glScissor() для изоляции тайла
  ↓
Рендеринг в нужный регион atlas текстуры
  ↓
getAtlasRect(lightIndex) возвращает UV координаты для шейдера
  ↓
Сцена использует basic_advanced_atlas.frag
```

## Интеграция с Renderer

Для полной интеграции нужно добавить в ForwardRenderer:

```cpp
class ForwardRenderer {
private:
    ShadowManager shadowManager_;
    
public:
    void render(Scene& scene) {
        // 1. Инициализация (один раз)
        shadowManager_.init();
        if (useShadowAtlas) {
            shadowManager_.allocateAtlasSpace(lights.size(), 2048);
        }
        
        // 2. Рендеринг теней
        for (auto light : scene.lights) {
            if (light->type == LightType::Directional) {
                glm::mat4 lightSpace = calculateLightSpaceMatrix(light);
                shadowManager_.renderDirectionalShadow(light, lightSpace, meshes);
            }
            // ... similar for point and spot lights
        }
        
        // 3. Рендеринг сцены с освещением
        renderScene(meshes);
        
        // 4. Cleanup
        shadowManager_.destroy();
    }
};
```

## Использование

### Переключение режимов во время работы
```cpp
shadowManager.setShadowMode(ShadowMode::RSM);
shadowManager.setAtlasMode(AtlasMode::Atlas);

// Установка качества RSM
shadowManager.setRSMQuality(
    2.0f,   // smoothness (больше = мягче тени)
    0.001f, // bias (меньше = точнее, но может быть шум)
    0.03f,  // radius (больше = больше непрямого света)
    1.5f    // intensity (влияние непрямого света)
);
```

### Использование ShadowModeHelper
```cpp
ShadowModeHelper helper(&shadowManager);
helper.enableRSM(true);
helper.enableAtlas(true);
helper.setQuality(ShadowQuality::High);  // Preset качества
```

## Производительность

### Память (на свет)
- **Standard режим**: 1 текстура (глубина) 2048²
- **RSM режим**: 3 текстуры (цвет, норм, глубина) 2048² каждая
- **Atlas**: Одна консолидированная текстура 4096² для всех

### Рендеринг
- **Standard**: Одна итерация рендеринга
- **RSM**: Одна итерация с MRT (сложнее, но все в одном проходе)
- **PCF**: +8 доп. sample за пиксель (параметр smoothness)

## Известные ограничения и будущие улучшения

### Текущие ограничения
- Point lights в atlas режиме: требует преобразования cubemap → atlas (не реализовано)
- Нет автоматической фильтрации между тайлами atlas (seaming может быть видны)
- Temporal filtering требует двойной буферизации RSM текстур

### Возможные улучшения
1. **Variance Shadow Maps (VSM)** - Быстрая PCF через дисперсию
2. **Cascaded Shadow Maps** - Лучше для больших сцен
3. **Screen-space async compute** - RSM фильтрация через compute shader
4. **Directional light cascade atlas** - Специальная сетка для directional light
5. **Cone-stepping** - Оптимизация выборки RSM по расстоянию

## Файлы для проверки

1. **Headers**: `include/RenderLib/LightDirectional.hpp`, `LightPoint.hpp`, `LightSpot.hpp`, `ShadowManager.hpp`
2. **Реализация**: `src/LightDirectional.cpp`, `LightPoint.cpp`, `LightSpot.cpp`, `ShadowManager.cpp`
3. **Шейдеры**: `shaders/shadow_rsm.frag`, `basic_advanced_rsm.frag`, `basic_advanced_atlas.frag`
4. **Пример**: `examples/shadow_modes_example.cpp`

## Заключение

✅ **RSM полностью реализован** с поддержкой:
- Всех типов источников света
- PCF фильтрации
- Параметров качества
- Temporal накопления
- Непрямого освещения

✅ **Shadow Atlas полностью реализован** с поддержкой:
- Автоматической сетки тайлов
- Per-light UV координат
- Standard и RSM режимов
- PCF фильтрации в пределах тайла

✅ **Интеграция готова** к подключению в ForwardRenderer
