### Общие принципы проекта
разработка библиотеки RenderLib на C++ 23. Используется OpenGL 3.3.
из библиотек используется glad и glm.
для загрузки мешей используется tinyobjloader.
для загрузки текстур используется stb image.
оконная система не нужна, так как это библиотека. также здесь не нужно использовать cmake
максимально используется STL
Освещение реализовано как Forward Rendering и Blinn-Phong модель.
Там где это возможно, используется Uniform Buffer Object и Samplers Object

### Основная задача
Библиотека нужна для разработки трехмерной игры от первого лица в открытом мире.

### Требования
Типы источников света: 4 Directional, 4 Point и 4 Spot lights. При этом для каждого типа нужен свой класс. Не делать общий
для всех источников света (включая point) свои карты теней. При этом возможность как для каждого источника создавать свои карты теней, так и согласно настроек переключаться на shadow atlas.
для теней реализовать PCF, CSM. Также опцией сделать возможность включать или отключать RSM. RSM делать без вычислительных шейдеров.
текстуры: diffuse, specular, normal
модель из мешей
поддержка Parallax Occlusion Mapping и Normal Mapping
поддержка SSAO
поддержка SSR
Post-Processing: Bloom, volumetric fog, IBL, color grading
Performance Optimization: Frustum culling, Hi-Z occlusion culling, GPU instancing, object LOD, transparency sorting