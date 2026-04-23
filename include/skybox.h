// ============================================================
// Skybox Class - Renders a star-field background sphere
// Uses an inverted sphere with a panoramic star texture
// ============================================================
#pragma once

#include <glad/glad.h>
#include <string>

class Sphere;
class Shader;
class Camera;

class Skybox {
public:
    Skybox();
    ~Skybox();

    /// Load the star field texture
    bool loadTexture(const std::string& path);

    /// Generate a procedural star field if no texture is available
    void generateProceduralTexture();

    /// Render the skybox
    void draw(Shader& shader, const Camera& camera, int screenWidth, int screenHeight) const;

private:
    unsigned int textureID;
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    bool loaded;

    /// Build a large inverted sphere for the skybox
    void buildSkyboxSphere(unsigned int sectors = 64, unsigned int stacks = 32);
};
