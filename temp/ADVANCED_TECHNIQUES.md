# Advanced Rendering Techniques in RenderLib

## 1. Instancing (SSBO-based)

### What is it?
Instancing allows rendering **many copies of the same mesh** with different transforms in a single draw call, instead of calling draw for each instance.

### How it works in RenderLib:
- **SSBO (Shader Storage Buffer Object)**: Stores array of `InstanceData` (transform + color)
- Vertex shader reads: `instances[gl_InstanceID]` to get per-instance transform
- CPU calls: `glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount)`

### Usage Example:
```cpp
RenderLib::InstanceBuffer instBuffer;
instBuffer.create(10000);  // max 10k instances

std::vector<RenderLib::InstanceData> instances;
for(int i = 0; i < 100; ++i) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 2.0f, 0, 0));
    instances.push_back({model, glm::vec4(1.0f)});
}

instBuffer.updateData(instances);
instBuffer.bindSSBO(3);  // binding point 3

// In shader: layout(std430, binding = 3) readonly buffer InstanceBuffer { InstanceData instances[]; };
// In vertex: InstanceData inst = instances[gl_InstanceID];
```

### Performance:
- **Before**: 1000 meshes = 1000 draw calls
- **After**: 1000 instances = 1 draw call
- **Speedup**: 10-100x faster on modern GPUs

### Limitations:
- All instances share the same mesh and shader
- Material variations stored in SSBO (color, texture indices, etc.)

---

## 2. Frustum Culling

### What is it?
Removes **off-screen objects** before rendering to save GPU time.

### How it works:
1. Extract 6 planes from view-projection matrix (left, right, top, bottom, near, far)
2. Test each mesh's AABB (Axis-Aligned Bounding Box) against all planes
3. Skip rendering if AABB is completely outside any plane

### Mathematical basis:
- View-projection matrix encodes the camera frustum
- Each plane is extracted from rows/columns of the matrix
- Point-to-plane distance: `d = dot(point, plane.normal) + plane.distance`
- AABB test: find closest point on box to each plane, check if it's behind (negative distance)

### Usage:
```cpp
// After updating camera (view/proj matrices)
glm::mat4 viewProj = projection * view;
renderer.updateFrustum(viewProj);
renderer.enableFrustumCulling(true);

// When submitting meshes
glm::vec3 aabbMin(-1, -1, -1), aabbMax(1, 1, 1);
if(renderer.isMeshInFrustum(aabbMin, aabbMax)) {
    renderer.submitMesh(mesh, material, modelMatrix);
}
```

### Performance Impact:
- 1000 off-screen objects: 0 draw calls instead of 1000
- CPU-side culling: < 1ms for 10k objects
- GPU-side savings: can be 50-90% depending on scene complexity

### When NOT to use:
- Small scenes (< 1k objects)
- Scenes where most objects are visible
- High camera movement (frustum changes rapidly)

---

## 3. Transparency + Depth Sorting

### What is it?
Rendering semi-transparent objects **back-to-front** so blending works correctly.

### The Problem:
Alpha blending requires **order-dependent** rendering:
```
Result = Source.rgb * Source.a + Dest.rgb * (1 - Source.a)
```
If you blend front-to-back, closer transparent objects block the view of further ones (incorrect).

### Solution in RenderLib:

1. **Separate Queues**: Divide meshes into opaque and transparent
2. **Sort by Depth**: Calculate distance of each transparent mesh center to camera
3. **Render back-to-front**: Draw furthest first, nearest last

### Usage:
```cpp
// Submit meshes
renderer.submitMesh(opaqueMesh, material, transform, false);        // opaque
renderer.submitMesh(glassMesh, glassMaterial, transform, true);    // transparent

// In endFrame(), automatically:
// 1. Render all opaque meshes (with depth write)
// 2. Sort transparent by distance
// 3. Render transparent back-to-front (with alpha blending)
```

### Implementation Details:
```cpp
// In fragment shader for transparent objects
vec4 texColor = texture(uDiffuse, vTex);
if(texColor.a < 0.01) discard;  // Early discard

FragColor = vec4(color, texColor.a);  // Output with alpha
```

### In render loop:
```cpp
// Opaque pass
glDisable(GL_BLEND);
for(auto& mesh : opaqueQueue) { /* render */ }

// Transparent pass
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glDepthMask(GL_FALSE);  // Don't write to depth buffer
for(auto& mesh : sortedTransparentQueue) { /* render */ }
glDepthMask(GL_TRUE);
glDisable(GL_BLEND);
```

### Performance Cost:
- CPU sort: O(n log n) per frame (acceptable for 100-1000 transparent objects)
- GPU overhead: minimal (just different blend state)

### Limitations:
- Can't blend translucent objects with each other perfectly (depth-sorting has limitations)
- Solution: Order-Independent Transparency (OIT) using linked lists (advanced technique)

---

## 4. Deferred SSAO Fallback

### What is Deferred Rendering?
Traditional **deferred rendering**:
1. First pass: render G-buffer (position, normal, albedo, etc.) to textures
2. Second pass: calculate lighting using G-buffer data
3. Advantage: many lights, one light-pass per pixel
4. Disadvantage: complex, memory-intensive

### Deferred "Fallback" in RenderLib:
A **lightweight deferred technique** that only reconstructs position/normal from depth:
- No full G-buffer needed
- Only depth texture (already available)
- Reconstruct view-space position from depth + inverse projection
- Reconstruct normal from depth gradient

### How it works:

#### Step 1: Reconstruct Position
```glsl
vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
vec4 viewPos = invProjection * ndc;
viewPos /= viewPos.w;
vec3 position = viewPos.xyz;
```

#### Step 2: Reconstruct Normal
Sample depth at neighboring pixels, compute gradient:
```glsl
float depthUp = texture(depthTex, uv + vec2(0, pixelSize)).r;
float depthLeft = texture(depthTex, uv - vec2(pixelSize, 0)).r;

vec3 posUp = reconstructPosition(uv + vec2(0, pixelSize), depthUp);
vec3 posLeft = reconstructPosition(uv - vec2(pixelSize, 0), depthLeft);
vec3 normal = normalize(cross(posUp - pos, posLeft - pos));
```

#### Step 3: Use for Lighting/SSAO
Now you have position and normal, can do:
- SSAO (as in `ssao_deferred.frag`)
- Screen-space reflections
- Any post-process that needs geometry info

### Why is it called "Fallback"?
- **Primary method**: Forward rendering with full material info (normal maps, etc.)
- **Fallback method**: If you need deferred-like passes (SSAO, reflections), reconstruct geometry from depth alone
- Hybrid approach: best of both worlds

### Usage:
```cpp
// After rendering scene to depth buffer
glUseProgram(ssaoShaderDeferredFallback);
glUniformMatrix4fv(locInvProj, 1, GL_FALSE, &invProjection[0][0]);
glUniformMatrix4fv(locInvView, 1, GL_FALSE, &invView[0][0]);
glUniform2f(locScreenSize, width, height);

// Bind depth texture
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, depthTexture);
glUniform1i(locDepth, 0);

// Render fullscreen quad
renderFullscreenQuad();
```

### Accuracy Issues:
- Reconstructed normal is **smooth**, loses high-frequency details
- Position reconstruction can have **precision issues** at extreme angles
- Use for **global** effects (SSAO, fog) rather than **local** ones (bumps, details)

### Performance:
- **Deferred G-buffer**: store 4+ textures per pixel (expensive memory bandwidth)
- **Deferred fallback**: read 1 depth texture, compute (cheap)
- **Savings**: ~60% memory bandwidth vs full deferred, ~10% overhead vs forward

---

## Summary Table

| Technique | Cost | Benefit | When to Use |
|-----------|------|---------|------------|
| **Instancing** | Low (1 draw call) | Render 1000+ meshes | Forests, particle systems, crowds |
| **Frustum Culling** | CPU low (~1ms/10k) | Skip invisible objects | Large scenes (>1k objects) |
| **Transparency Sort** | CPU O(n log n) | Correct blending | Scenes with 100-1000 transparent objects |
| **Deferred Fallback** | Medium (2 passes) | Post-process with geometry | SSAO, SSR without full G-buffer |

