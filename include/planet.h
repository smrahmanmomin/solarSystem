// ============================================================
// Planet Class - Represents a celestial body in the solar system
// Handles orbit revolution, axial rotation, and rendering
// ============================================================
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Sphere;
class Shader;
class Ring;

#include <memory>

class Planet {
public:
    std::string name;

    // Physical properties
    float radius;           // Visual radius (scaled)
    float orbitRadius;      // Distance from Sun
    float rotationSpeed;    // Degrees per second (axial)
    float revolutionSpeed;  // Degrees per second (orbital)
    float axialTilt;        // Tilt of rotation axis (degrees)
    float oblateness;       // Flattening at the poles (default 1.0)

    // Current animation state
    float currentRotation;    // Current axial rotation angle
    float currentRevolution;  // Current orbital angle

    // Orbit Rendering ID
    size_t orbitId;

    // Texture
    unsigned int textureID;
    bool hasTexture;

    // Ring (Saturn)
    bool hasRing;
    float ringInnerRadius;
    float ringOuterRadius;
    unsigned int ringTextureID;
    std::shared_ptr<Ring> ringMesh; // shared_ptr since Planet can be copied in std::vector

    // Color fallback (if no texture)
    glm::vec3 color;

    /// Construct a planet with all orbital parameters
    Planet(const std::string& name, float radius, float orbitRadius,
           float rotSpeed, float revSpeed, float axialTilt,
           const glm::vec3& color = glm::vec3(0.5f));

    /// Load texture from file (returns true on success)
    bool loadTexture(const std::string& path);

    /// Load ring texture from file
    bool loadRingTexture(const std::string& path);

    /// Update planet and its moons based on delta time
    void update(float deltaTime, float speedMultiplier);

    /// Get the matrix containing just the translation for the orbit (parent's position)
    glm::mat4 getOrbitMatrix(const glm::mat4& parentOrbitMatrix = glm::mat4(1.0f)) const;

    /// Get the full model transformation matrix
    glm::mat4 getModelMatrix(const glm::mat4& parentOrbitMatrix = glm::mat4(1.0f)) const;

    /// Get current world-space position
    glm::vec3 getPosition(const glm::mat4& parentOrbitMatrix = glm::mat4(1.0f)) const;

    /// Initialize the ring mesh if the planet has a ring
    void initRing();

    /// Draw the planet and its moons using provided sphere mesh and shader
    void draw(const Sphere& sphere, Shader& planetShader, Shader* ringShader = nullptr, const glm::mat4& parentOrbitMatrix = glm::mat4(1.0f)) const;

    /// Add a moon to this planet
    void addMoon(const Planet& moon) { moons.push_back(moon); }

    std::vector<Planet> moons;

    // ── 2D Transformation Demos (Academic Requirement) ─────
    /// Get model matrix with reflection (mirror across a plane)
    glm::mat4 getReflectedModelMatrix(const glm::vec3& planeNormal) const;

    /// Get model matrix with shear transformation
    glm::mat4 getShearedModelMatrix(float shearFactor) const;
};
