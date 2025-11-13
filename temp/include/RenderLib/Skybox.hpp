#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace RenderLib {

class CubemapTexture {
public:
    CubemapTexture();
    ~CubemapTexture();

    // Load cubemap from 6 individual files (right, left, top, bottom, front, back)
    bool loadFromFiles(const std::vector<std::string>& faces);
    
    // Load cubemap from pre-loaded pixel data (for each face)
    bool loadFromMemory(int width, int height, const std::vector<void*>& faceData, GLenum format = GL_RGB);
    
    void bind(unsigned unit) const;
    GLuint id() const { return texture_; }

private:
    GLuint texture_{0};
};

class Skybox {
public:
    Skybox();
    ~Skybox();

    bool init(const CubemapTexture* cubemap);
    void render(GLuint shaderProgram, const glm::mat4& projection, const glm::mat4& view);

private:
    GLuint vao_{0}, vbo_{0};
    const CubemapTexture* cubemap_{nullptr};
};

} // namespace RenderLib
