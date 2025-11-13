# RenderLib Shadow Modes Documentation

## Overview

RenderLib Phase 6 Extended now includes two advanced shadow rendering systems with optional modes:

1. **Reflective Shadow Maps (RSM)** - Captures color and normal information from light viewpoint
2. **Shadow Atlas** - Consolidates multiple shadow maps into single texture

## Features

### Reflective Shadow Maps (RSM)

**Description**: RSM captures not only depth but also color and surface normal information from the light's perspective. This enables:
- Indirect shadow calculation
- More realistic shadow color bleeding
- Normal-based shadow filtering
- Better soft shadow approximation

**Technical Details**:
- Uses Multiple Render Targets (MRT) with 2 color attachments
- Attachment 0: Color information (RGBA16F)
- Attachment 1: Normal information (RGBA16F)
- Depth attachment: Standard depth map (DEPTH_COMPONENT32F)
- Available for: Directional, Point, and Spot lights

**Usage**:
```cpp
ShadowManager shadowMgr;
shadowMgr.init();

// Enable RSM mode
shadowMgr.setShadowMode(ShadowMode::RSM);

// Render shadows with RSM
DirectionalLight* light = ...;
glm::mat4 lightSpace = ...;
shadowMgr.renderDirectionalShadow(light, lightSpace, meshes);

// Bind RSM textures for sampling
// In shader:
// - Bind light.rsmColorMap to sample color
// - Bind light.rsmNormalMap to sample normals
// - Bind light.depthMapFBO for depth comparison
```

### Shadow Atlas

**Description**: Shadow atlas consolidates multiple shadow maps into a single texture. Benefits:
- Reduced memory usage (single large texture vs many individual textures)
- Reduced texture binding overhead
- Better cache locality for GPU
- Efficient multi-light shadow rendering

**Technical Details**:
- Single large texture dimensions: 2048x2048 (default)
- Automatically calculates grid layout (e.g., 2x2 for 4 lights)
- Each light gets rect mapping: `vec4(x, y, width, height)` in normalized [0,1] space
- Supports both Standard and RSM modes

**Usage**:
```cpp
ShadowManager shadowMgr;
shadowMgr.init();

// Allocate atlas space for 4 lights at 1024px resolution
shadowMgr.allocateAtlasSpace(4, 1024);

// Enable Atlas mode
shadowMgr.setAtlasMode(AtlasMode::Atlas);

// Get atlas rect for specific light
glm::vec4 atlasRect = shadowMgr.getAtlasRect(lightIndex);

// In shader, remap coordinates:
// vec2 atlasCoord = lightSpaceUV * atlasRect.zw + atlasRect.xy;
```

## Mode Selection

### Shadow Modes

```cpp
enum class ShadowMode {
    Standard,    // Depth-only PCF (default)
    RSM          // Reflective Shadow Maps (color + normal)
};

shadowManager->setShadowMode(ShadowMode::Standard);  // Traditional
shadowManager->setShadowMode(ShadowMode::RSM);      // Advanced indirect
```

### Atlas Modes

```cpp
enum class AtlasMode {
    Individual,  // Separate shadow maps per light (default)
    Atlas        // Consolidated texture
};

shadowManager->setAtlasMode(AtlasMode::Individual);  // More VRAM, less overhead
shadowManager->setAtlasMode(AtlasMode::Atlas);      // Less VRAM, more texture ops
```

## Shader Integration

### Standard Mode Shader

```glsl
#version 450 core

uniform sampler2D shadowMap;
uniform vec4 fragPosLightSpace;

float shadow = texture(shadowMap, coord).r;
if (fragDepth > shadow + bias) {
    // In shadow
}
```

### RSM Mode Shader

See `shaders/basic_advanced_rsm.frag`:
- Samples both color and normal maps
- Applies RSM-aware shadow filtering
- Indirect lighting calculation

```glsl
vec3 rsmColor = sampleRSM(lightSpacePos).rgb;
vec3 indirectLight = rsmColor * baseColor * 0.3;
```

### Atlas Mode Shader

See `shaders/basic_advanced_atlas.frag`:
- Maps light-space coordinates to atlas tile
- Uses atlasRect for region mapping
- Single texture access for all lights

```glsl
uniform vec4 atlasRect;
vec2 atlasCoord = atlasRect.xy + lightSpaceUV * atlasRect.zw;
float depth = texture(atlasTexture, atlasCoord).r;
```

## Implementation Details

### Light Classes

Each light type (`DirectionalLight`, `PointLight`, `SpotLight`) contains:

**Standard Mode Fields**:
- `depthMapFBO` / `cubeShadowFBO` - Framebuffer object
- `depthMap` / `cubeShadowMap` - Depth texture

**RSM Mode Fields**:
- `rsmFBO` - RSM framebuffer
- `rsmColorMap` - Color information texture
- `rsmNormalMap` - Normal information texture
- `rsmDepthMap` - Depth for RSM (cubemap variant: `rsmCubeMap`, `rsmColorCube`)

**Atlas Fields**:
- `useAtlas` - Enable atlas mode flag
- `atlasRect` - Region in atlas (x, y, width, height)

### Shadow Manager

Central class managing all shadow rendering:

```cpp
class ShadowManager {
public:
    void setShadowMode(ShadowMode mode);
    void setAtlasMode(AtlasMode mode);
    
    void renderDirectionalShadow(...);
    void renderPointShadow(...);
    void renderSpotShadow(...);
    
    void allocateAtlasSpace(uint32_t lightCount, uint32_t resolution);
    glm::vec4 getAtlasRect(uint32_t lightIndex) const;
};
```

## Performance Considerations

### RSM vs Standard

**RSM Pros**:
- More realistic indirect shadows
- Better soft shadow approximation
- Color bleeding effects

**RSM Cons**:
- ~2x memory (dual color attachments)
- ~2x bandwidth for texture writes
- More complex sampling

**Recommendation**: Use RSM only for key lights (directional, primary spotlight)

### Individual vs Atlas

**Individual Pros**:
- Simpler implementation
- No coordinate remapping
- Better for varying resolutions

**Individual Cons**:
- More texture bindings
- Higher VRAM for many lights
- More state changes

**Atlas Pros**:
- Memory efficient
- Fewer texture bindings
- Better caching

**Atlas Cons**:
- Coordinate transformation overhead
- Fixed resolution per light
- Grid layout constraints

## Example: Switching Modes at Runtime

```cpp
void toggleShadowMode(ShadowManager& mgr) {
    if (mgr.getShadowMode() == ShadowMode::Standard) {
        mgr.setShadowMode(ShadowMode::RSM);
        std::cout << "Switched to RSM\n";
    } else {
        mgr.setShadowMode(ShadowMode::Standard);
        std::cout << "Switched to Standard\n";
    }
}

void toggleAtlasMode(ShadowManager& mgr) {
    if (mgr.getAtlasMode() == AtlasMode::Individual) {
        mgr.setAtlasMode(AtlasMode::Atlas);
        std::cout << "Switched to Atlas\n";
    } else {
        mgr.setAtlasMode(AtlasMode::Individual);
        std::cout << "Switched to Individual\n";
    }
}
```

## Files Reference

### Headers
- `include/RenderLib/ShadowManager.hpp` - Shadow manager interface
- `include/RenderLib/LightDirectional.hpp` - Directional light (enums, RSM fields)
- `include/RenderLib/LightPoint.hpp` - Point light (cubemap RSM)
- `include/RenderLib/LightSpot.hpp` - Spot light (2D RSM)

### Source
- `src/ShadowManager.cpp` - Shadow rendering implementation
- `src/LightDirectional.cpp` - RSM initialization and cleanup
- `src/LightPoint.cpp` - Cubemap RSM methods
- `src/LightSpot.cpp` - 2D RSM methods

### Shaders
- `shaders/shadow_rsm.vert` - RSM vertex shader
- `shaders/shadow_rsm.frag` - RSM fragment shader (MRT output)
- `shaders/shadow_atlas.vert` - Atlas vertex shader
- `shaders/shadow_atlas.frag` - Atlas fragment shader
- `shaders/basic_advanced_rsm.frag` - RSM sampling shader
- `shaders/basic_advanced_atlas.frag` - Atlas sampling shader

### Examples
- `examples/shadow_modes_example.cpp` - Complete mode switching example

## Compatibility

- **OpenGL Version**: Requires OpenGL 4.5+ (MRT support)
- **Extensions**: 
  - `ARB_draw_buffers` (MRT)
  - `ARB_depth_texture` (Depth comparison)
- **Backward Compatibility**: Existing standard shadow code unchanged

## Future Enhancements

Potential improvements:
1. **Variance Shadow Maps (VSM)** - Reduced shadow acne, better filtering
2. **Cascaded Shadow Maps** - For directional lights with better far-field quality
3. **Cache-oblivious tiling** - Better atlas layout for variable-sized regions
4. **Deferred Shadow Atlasing** - Lazy allocation during frame
5. **Sparse textures** - Virtual texture support for very large atlases
