# RenderLib — Forward Rendering Library (C++23 + OpenGL 3.3)

A minimal, header-rich rendering library for forward rendering using OpenGL 3.3 and C++23. Designed as a library-only (no window management) with support for advanced techniques like shadow mapping, normal mapping, bloom, and screen-space reflections.

## Features

### Core Rendering
- **Forward Rendering**: Single-pass lighting with Blinn-Phong model
- **Lighting**: Up to 4 directional, 4 point, and 4 spot lights (separate classes per light type)
- **Texturing**: Diffuse, specular, and normal maps with full bump mapping support

### Advanced Surface Shading
- **Parallax Mapping**: Depth-aware UV offset for surface detail without extra geometry (basic & layered)
- **Normal Mapping**: Full TBN-space normal mapping with tangent calculation
- **Bump Mapping**: Height-to-normal conversion via parallax

### Shadows
- **Standard Shadow Maps**:
  - Directional & Spot lights: 2D depth maps with PCF (3x3 filtering)
  - Point lights: Cubemap depth with soft-shadow approximation
- **Cascade Shadow Maps**: 3-tier cascading for directional lights (better far-view quality)
- **All calculations in fragment shader** (no vertex attribute passing)

### Postprocessing
- **HDR Rendering**: RGBA16F framebuffer with Reinhard tone mapping
- **Bloom**: Bright-pass extraction + separable Gaussian blur (5-sample)
- **FXAA**: Fast approximate anti-aliasing for smooth edges
- **SSAO**: Screen-space ambient occlusion (16-sample kernel)
- **Screen-Space Reflections**: Cheap ray-marched reflections

### Environment
- **Skybox**: Cubemap-based with proper depth handling
- **Volumetric Fog**: Distance-based with configurable height zone

### Performance
- **Efficient UBOs**: `std140` layout, 16KB for all lights
- **Sampler Objects**: Independent texture/filtering state
- **Instancing-ready**: Structure supports batch rendering
- **HDR Pipeline**: Single 16-bit float target for full dynamic range

## Project Structure

```
RenderLib/
├── CMakeLists.txt
├── include/RenderLib/
│   ├── Shader.hpp, Texture.hpp, Sampler.hpp, UBO.hpp
│   ├── Mesh.hpp, Renderer.hpp
│   ├── LightDirectional.hpp, LightPoint.hpp, LightSpot.hpp
│   ├── Skybox.hpp                    (cubemap + IBL)
│   ├── CascadeShadow.hpp             (tiered shadow maps)
│   └── Advanced.hpp                  (parallax settings)
├── src/
│   ├── Shader.cpp, Texture.cpp, Sampler.cpp, UBO.cpp, Mesh.cpp
│   ├── Renderer.cpp
│   ├── LightDirectional.cpp, LightPoint.cpp, LightSpot.cpp
│   ├── Skybox.cpp, CascadeShadow.cpp
│   └── [utilities]
└── shaders/
    ├── basic.vert / basic.frag                (simple Blinn-Phong)
    ├── basic_advanced.vert / basic_advanced.frag  (parallax + cascades)
    ├── shadow_*.vert / shadow_depth.frag      (depth-only passes)
    ├── skybox.vert / skybox.frag              (cubemap rendering)
    ├── fxaa.frag, blur.frag, extract_bright.frag  (postprocessing)
    ├── post_bloom.frag                        (HDR tone map + bloom)
    ├── ssao.frag                              (ambient occlusion)
    └── ssr.frag                               (screen-space reflections)
```

## Building

### Prerequisites

- **C++23 compiler** (MSVC, Clang, GCC with C++23 support)
- **glad**: OpenGL loader (you provide source/headers)
- **GLM**: Header-only math library
- **CMake** 3.15 or later

### Build Steps

```bash
mkdir build
cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Usage

Since this is a library-only package, you must:

1. **Create an OpenGL context** (via GLFW, SDL, or your framework)
2. **Initialize glad**: `gladLoadGL()`
3. **Compile shaders** and load textures
4. **Create lights** and initialize shadow maps
5. **Use ForwardRenderer** to render

### Minimal Example (Pseudocode)

```cpp
#include "RenderLib/Renderer.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/Mesh.hpp"
#include <glm/glm.hpp>

int main() {
    // Assume OpenGL context and glad initialized
    
    // Initialize renderer with window dimensions
    RenderLib::ForwardRenderer renderer;
    renderer.init(1920, 1080);
    
    // Create a directional light and initialize shadow map
    RenderLib::DirectionalLight dirLight;
    dirLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    dirLight.color = glm::vec3(1.0f);
    dirLight.intensity = 1.0f;
    dirLight.initShadowMap();  // creates 2048x2048 depth map
    
    // Load shader
    RenderLib::Shader mainShader;
    mainShader.compileFromFiles("shaders/basic.vert", "shaders/basic.frag");
    
    // Create mesh (example: cube)
    std::vector<RenderLib::Vertex> vertices = { /* ... */ };
    std::vector<unsigned int> indices = { /* ... */ };
    RenderLib::Mesh mesh;
    mesh.create(vertices, indices);
    
    // Create material with textures
    RenderLib::Texture2D diffuseTex;
    diffuseTex.fromMemory(512, 512, GL_RGB, diffuse_data);
    
    RenderLib::Material material;
    material.diffuse = &diffuseTex;
    material.specular = &specularTex;
    material.normal = &normalTex;
    material.shininess = 32.0f;
    
    // Rendering loop
    while(/* not done */) {
        renderer.beginFrame();
        
        mainShader.use();
        // Set view/proj matrices, camera position, light UBO data
        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &proj[0][0]);
        glUniform3fv(locViewPos, 1, &camPos.x);
        
        // Submit mesh for rendering
        glm::mat4 model = glm::mat4(1.0f);
        renderer.submitMesh(mesh, material, model);
        
        renderer.endFrame();
    }
    
    return 0;
}
```

## UBO Layout (std140)

The `Lights` uniform block (binding point 1) follows this layout:

```glsl
layout(std140) uniform Lights {
    DirLight dirLights[4];      // 32 bytes each (vec3 + 1 pad, vec3 + intensity)
    PointLight pointLights[4];  // 48 bytes each
    SpotLight spotLights[4];    // 64 bytes each
    mat4 dirLightMatrices[4];   // 64 bytes each
    mat4 spotLightMatrices[4];  // 64 bytes each
};
```

Update this UBO via:
```cpp
RenderLib::UBO lights;
lights.create("Lights", 1, 16 * 1024);
lights.update(offset, size, data);
```

## Advanced Usage Tips

### Parallax Mapping
1. Store heightmap in normal texture's alpha channel (0=raised, 1=sunken)
2. Set `heightScale` uniform (0.05-0.15 typical)
3. Use `basic_advanced` shaders instead of `basic`

### Cascade Shadows
1. Create `CascadeShadowMap` instead of individual directional lights
2. Update splits based on view distance: `{0.01, 0.1, 0.5, 1000}`
3. Bind cascaded depth maps to `cascadeShadowMaps[lightIdx][cascade]`

### Skybox + IBL
```cpp
RenderLib::CubemapTexture cubemap;
cubemap.loadFromMemory(512, 512, {right, left, top, bottom, front, back}, GL_RGB);
RenderLib::Skybox skybox;
skybox.init(&cubemap);
// In render loop: skybox.render(skyboxShader, proj, view);
```

### SSAO Setup
1. Render G-buffer with positions, normals (needs depth prepass)
2. Run SSAO pass with kernel sampling
3. Blur result and apply multiplicatively to final color

### FXAA
- Apply as final post-process over HDR result
- Set `texelSize = 1.0 / vec2(width, height)`

## Notes

- **No window creation**: Consumers must provide OpenGL context
- **Image loading**: Use `stb_image` (not bundled) to load textures
- **Shadowing**: All shadow-map generation and sampling is CPU-managed and shader-executed
- **Postprocessing**: Bloom, SSAO, FXAA, SSR are post-passes over HDR color buffer
- **Extensibility**: Shader sources in `shaders/` directory for easy customization
- **Parallax Performance**: Use basic parallax (8 layers) for most cases; increase layers only when needed

## License

Public Domain / Unlicensed (MIT compatible)
