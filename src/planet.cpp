// ============================================================
// Planet Class Implementation
// Handles orbital mechanics, texture loading, and rendering
// Demonstrates 2D transformations: Translation, Rotation,
// Scaling, Reflection, and Shear
// ============================================================

#include "planet.h"
#include "sphere.h"
#include "shader.h"
#include "ring.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Planet::Planet(const std::string& name, float radius, float orbitRadius,
               float rotSpeed, float revSpeed, float axialTilt,
               const glm::vec3& color)
    : name(name)
    , radius(radius)
    , orbitRadius(orbitRadius)
    , rotationSpeed(rotSpeed)
    , revolutionSpeed(revSpeed)
    , axialTilt(axialTilt)
    , oblateness(1.0f)
    , currentRotation(0.0f)
    , currentRevolution(0.0f)
    , textureID(0)
    , hasTexture(false)
    , hasRing(false)
    , ringInnerRadius(0.0f)
    , ringOuterRadius(0.0f)
    , ringTextureID(0)
    , color(color)
{
}

bool Planet::loadTexture(const std::string& path) {
    // Load image using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "[Planet] WARNING: Failed to load texture: " << path
                  << " for " << name << std::endl;
        return false;
    }

    // Determine format
    GLenum format = GL_RGB;
    if (nrChannels == 1)      format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    // Create OpenGL texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    hasTexture = true;

    std::cout << "[Planet] Loaded texture for " << name
              << " (" << width << "x" << height << ")" << std::endl;
    return true;
}

bool Planet::loadRingTexture(const std::string& path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "[Planet] WARNING: Failed to load ring texture: " << path << std::endl;
        return false;
    }

    GLenum format = GL_RGBA;
    if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    glGenTextures(1, &ringTextureID);
    glBindTexture(GL_TEXTURE_2D, ringTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return true;
}

void Planet::initRing() {
    if (hasRing) {
        ringMesh = std::make_shared<Ring>(ringInnerRadius, ringOuterRadius, 128);
    }
}

void Planet::update(float deltaTime, float speedMultiplier) {
    // Update axial rotation (planet spinning on its own axis)
    // TRANSFORMATION: Rotation
    currentRotation += rotationSpeed * deltaTime * speedMultiplier;
    if (currentRotation > 360.0f) currentRotation -= 360.0f;

    // Update orbital revolution (planet moving around the Sun)
    // TRANSFORMATION: Translation (via rotation around origin)
    currentRevolution += revolutionSpeed * deltaTime * speedMultiplier;
    if (currentRevolution > 360.0f) currentRevolution -= 360.0f;

    // Update moons
    for (auto& moon : moons) {
        moon.update(deltaTime, speedMultiplier);
    }
}

glm::mat4 Planet::getOrbitMatrix(const glm::mat4& parentOrbitMatrix) const {
    glm::mat4 model = parentOrbitMatrix;
    float radAngle = glm::radians(currentRevolution);
    float x = orbitRadius * cos(radAngle);
    float z = orbitRadius * sin(radAngle);
    model = glm::translate(model, glm::vec3(x, 0.0f, z));
    return model;
}

glm::mat4 Planet::getModelMatrix(const glm::mat4& parentOrbitMatrix) const {
    glm::mat4 model = getOrbitMatrix(parentOrbitMatrix);

    // STEP 2 - ROTATION: Apply axial tilt
    model = glm::rotate(model, glm::radians(axialTilt), glm::vec3(0.0f, 0.0f, 1.0f));

    // STEP 3 - ROTATION: Spin planet on its axis
    model = glm::rotate(model, glm::radians(currentRotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // STEP 4 - SCALING: Apply planet size and oblateness
    model = glm::scale(model, glm::vec3(radius, radius * oblateness, radius));

    return model;
}

glm::vec3 Planet::getPosition(const glm::mat4& parentOrbitMatrix) const {
    glm::mat4 orbitMat = getOrbitMatrix(parentOrbitMatrix);
    return glm::vec3(orbitMat[3]); // 4th column is the translation
}

void Planet::draw(const Sphere& sphere, Shader& planetShader, Shader* ringShader, const glm::mat4& parentOrbitMatrix) const {
    // Bind texture if available
    planetShader.use();
    planetShader.setBool("useTexture", hasTexture);
    planetShader.setVec3("objectColor", color);

    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    // Calculate this planet's orbit matrix for its moons
    glm::mat4 currentOrbitMatrix = getOrbitMatrix(parentOrbitMatrix);

    // Set model matrix (includes Translation, Rotation, Scaling)
    planetShader.setMat4("model", getModelMatrix(parentOrbitMatrix));

    // Draw the sphere
    sphere.draw();

    // Draw ring
    if (hasRing && ringMesh && ringShader) {
        ringShader->use();
        
        glm::mat4 ringModel = getOrbitMatrix(parentOrbitMatrix);
        ringModel = glm::rotate(ringModel, glm::radians(axialTilt), glm::vec3(0.0f, 0.0f, 1.0f));
        // ringModel = glm::rotate(ringModel, glm::radians(currentRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        
        ringShader->setMat4("model", ringModel);
        
        if (ringTextureID) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ringTextureID);
            ringShader->setInt("ringTexture", 0);
        }
        
        ringMesh->draw();
    }

    // Draw moons
    for (const auto& moon : moons) {
        moon.draw(sphere, planetShader, ringShader, currentOrbitMatrix);
    }
}

// ── 2D Transformation Demos (Academic Requirement) ─────────────

glm::mat4 Planet::getReflectedModelMatrix(const glm::vec3& planeNormal) const {
    // TRANSFORMATION: Reflection
    // Reflect the planet across a plane defined by the normal vector
    // Reflection matrix: I - 2 * n * n^T
    glm::mat4 model = getModelMatrix();

    glm::mat4 reflection(1.0f);
    if (planeNormal.y != 0.0f) {
        // Reflect across XZ plane (mirror in Y)
        reflection[1][1] = -1.0f;
    } else if (planeNormal.x != 0.0f) {
        // Reflect across YZ plane (mirror in X)
        reflection[0][0] = -1.0f;
    } else if (planeNormal.z != 0.0f) {
        // Reflect across XY plane (mirror in Z)
        reflection[2][2] = -1.0f;
    }

    return reflection * model;
}

glm::mat4 Planet::getShearedModelMatrix(float shearFactor) const {
    // TRANSFORMATION: Shear
    // Apply shear transformation to distort the planet shape
    glm::mat4 model = getModelMatrix();

    glm::mat4 shear(1.0f);
    shear[1][0] = shearFactor;  // Shear Y with respect to X
    shear[2][0] = shearFactor * 0.5f;  // Slight shear Z with respect to X

    return model * shear;
}
