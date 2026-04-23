// ============================================================
// OrbitRenderer - Renders orbital paths using graphics algorithms
// Uses Midpoint Circle Algorithm to generate orbit points
// Uses DDA/Bresenham for axis and grid lines
// ============================================================
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;

class OrbitRenderer {
public:
    OrbitRenderer();
    ~OrbitRenderer();

    /// Generate orbit path for a planet using Midpoint Circle Algorithm
    /// Returns the index of the generated orbit
    size_t addOrbit(float radius, const glm::vec3& color);

    /// Generate 3D axis lines using DDA algorithm
    void generateAxes(float length);

    /// Generate grid lines using Bresenham algorithm
    void generateGrid(float size, float spacing);

    /// Render all orbits (deprecated)
    void drawOrbits(Shader& shader) const;

    /// Render a specific orbit
    void drawOrbit(size_t index, Shader& shader, const glm::mat4& model) const;

    /// Render axis lines
    void drawAxes(Shader& shader) const;

    /// Render grid
    void drawGrid(Shader& shader) const;

    /// Clear all orbit data
    void clear();

private:
    struct OrbitData {
        unsigned int VAO, VBO;
        unsigned int vertexCount;
        glm::vec3 color;
    };

    std::vector<OrbitData> orbits;

    // Axis lines
    unsigned int axisVAO, axisVBO;
    unsigned int axisVertexCount;
    bool axisGenerated;

    // Grid lines
    unsigned int gridVAO, gridVBO;
    unsigned int gridVertexCount;
    bool gridGenerated;
};
