// ============================================================
// Sphere Class - Generates a UV sphere mesh for planets
// Uses VAO/VBO/EBO for modern OpenGL rendering
// ============================================================
#pragma once

#include <glad/glad.h>
#include <vector>

class Sphere {
public:
    /// Create a UV sphere with given sector/stack counts
    /// More sectors/stacks = smoother sphere
    Sphere(unsigned int sectors = 72, unsigned int stacks = 36);
    ~Sphere();

    /// Draw the sphere using indexed rendering
    void draw() const;

    /// Get vertex count (for debugging)
    unsigned int getIndexCount() const { return indexCount; }

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    /// Generate sphere vertices and indices
    void buildMesh(unsigned int sectors, unsigned int stacks);
};
