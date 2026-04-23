// ============================================================
// OrbitRenderer Implementation
// Uses Midpoint Circle, DDA, and Bresenham algorithms
// to generate and render orbit paths, axes, and grid
// ============================================================

#include "orbit_renderer.h"
#include "shader.h"
#include "algorithms/midpoint_circle.h"
#include "algorithms/dda.h"
#include "algorithms/bresenham.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

OrbitRenderer::OrbitRenderer()
    : axisVAO(0), axisVBO(0), axisVertexCount(0), axisGenerated(false)
    , gridVAO(0), gridVBO(0), gridVertexCount(0), gridGenerated(false) {}

OrbitRenderer::~OrbitRenderer() {
    clear();
    if (axisGenerated) { glDeleteVertexArrays(1, &axisVAO); glDeleteBuffers(1, &axisVBO); }
    if (gridGenerated) { glDeleteVertexArrays(1, &gridVAO); glDeleteBuffers(1, &gridVBO); }
}

size_t OrbitRenderer::addOrbit(float radius, const glm::vec3& color) {
    // Use MIDPOINT CIRCLE ALGORITHM to generate orbit points
    auto points = algorithms::midpoint_circle_orbit(radius, 800);
    if (points.empty()) return (size_t)-1;

    // Build vertex data: position (3) + color (3)
    std::vector<float> vertices;
    vertices.reserve(points.size() * 6);
    for (auto& p : points) {
        vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
        vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    }

    OrbitData od;
    od.vertexCount = (unsigned int)points.size();
    od.color = color;

    glGenVertexArrays(1, &od.VAO);
    glGenBuffers(1, &od.VBO);
    glBindVertexArray(od.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, od.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    orbits.push_back(od);
    return orbits.size() - 1;
}

void OrbitRenderer::generateAxes(float length) {
    // Use DDA ALGORITHM to generate axis line points
    auto xAxis = algorithms::dda_line_3d(glm::vec3(-length, 0, 0), glm::vec3(length, 0, 0), 200);
    auto yAxis = algorithms::dda_line_3d(glm::vec3(0, -length, 0), glm::vec3(0, length, 0), 200);
    auto zAxis = algorithms::dda_line_3d(glm::vec3(0, 0, -length), glm::vec3(0, 0, length), 200);

    std::vector<float> vertices;
    // X axis (red)
    for (auto& p : xAxis) {
        vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
        vertices.push_back(1.0f); vertices.push_back(0.2f); vertices.push_back(0.2f);
    }
    // Y axis (green)
    for (auto& p : yAxis) {
        vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
        vertices.push_back(0.2f); vertices.push_back(1.0f); vertices.push_back(0.2f);
    }
    // Z axis (blue)
    for (auto& p : zAxis) {
        vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
        vertices.push_back(0.2f); vertices.push_back(0.2f); vertices.push_back(1.0f);
    }

    axisVertexCount = (unsigned int)(xAxis.size() + yAxis.size() + zAxis.size());

    if (axisGenerated) { glDeleteVertexArrays(1, &axisVAO); glDeleteBuffers(1, &axisVBO); }
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    axisGenerated = true;
}

void OrbitRenderer::generateGrid(float size, float spacing) {
    // Use BRESENHAM ALGORITHM to generate grid lines
    std::vector<float> vertices;
    float halfSize = size / 2.0f;

    // Lines parallel to X axis
    for (float z = -halfSize; z <= halfSize; z += spacing) {
        auto pts = algorithms::bresenham_line_3d(
            glm::vec3(-halfSize, 0, z), glm::vec3(halfSize, 0, z));
        for (auto& p : pts) {
            vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
            vertices.push_back(0.15f); vertices.push_back(0.15f); vertices.push_back(0.2f);
        }
    }
    // Lines parallel to Z axis
    for (float x = -halfSize; x <= halfSize; x += spacing) {
        auto pts = algorithms::bresenham_line_3d(
            glm::vec3(x, 0, -halfSize), glm::vec3(x, 0, halfSize));
        for (auto& p : pts) {
            vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
            vertices.push_back(0.15f); vertices.push_back(0.15f); vertices.push_back(0.2f);
        }
    }

    gridVertexCount = (unsigned int)(vertices.size() / 6);

    if (gridGenerated) { glDeleteVertexArrays(1, &gridVAO); glDeleteBuffers(1, &gridVBO); }
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    gridGenerated = true;
}

void OrbitRenderer::drawOrbits(Shader& shader) const {
    shader.use();
    glm::mat4 model(1.0f);
    shader.setMat4("model", model);
    shader.setFloat("alpha", 0.4f);
    for (auto& o : orbits) {
        glBindVertexArray(o.VAO);
        glDrawArrays(GL_LINE_LOOP, 0, o.vertexCount);
    }
    glBindVertexArray(0);
}

void OrbitRenderer::drawOrbit(size_t index, Shader& shader, const glm::mat4& model) const {
    if (index >= orbits.size()) return;
    shader.use();
    shader.setMat4("model", model);
    shader.setFloat("alpha", 0.4f);
    glBindVertexArray(orbits[index].VAO);
    glDrawArrays(GL_LINE_LOOP, 0, orbits[index].vertexCount);
    glBindVertexArray(0);
}

void OrbitRenderer::drawAxes(Shader& shader) const {
    if (!axisGenerated) return;
    shader.use();
    glm::mat4 model(1.0f);
    shader.setMat4("model", model);
    shader.setFloat("alpha", 0.6f);
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_POINTS, 0, axisVertexCount);
    glBindVertexArray(0);
}

void OrbitRenderer::drawGrid(Shader& shader) const {
    if (!gridGenerated) return;
    shader.use();
    glm::mat4 model(1.0f);
    shader.setMat4("model", model);
    shader.setFloat("alpha", 0.2f);
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_POINTS, 0, gridVertexCount);
    glBindVertexArray(0);
}

void OrbitRenderer::clear() {
    for (auto& o : orbits) {
        glDeleteVertexArrays(1, &o.VAO);
        glDeleteBuffers(1, &o.VBO);
    }
    orbits.clear();
}
