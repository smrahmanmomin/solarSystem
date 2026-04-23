// ============================================================
// Ring Class - Generates a 2D disc mesh for planetary rings
// ============================================================
#pragma once

#include <glad/glad.h>
#include <vector>

class Ring {
public:
    Ring(float innerRadius = 0.5f, float outerRadius = 1.0f, unsigned int segments = 128);
    ~Ring();

    void draw() const;

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    void buildMesh(unsigned int segments);
};
