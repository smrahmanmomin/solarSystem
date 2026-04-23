// ============================================================
// Sphere Class Implementation
// Generates a UV sphere mesh with positions, normals, and UVs
// Uses VAO/VBO/EBO for modern OpenGL indexed rendering
// ============================================================

#include "sphere.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Sphere::Sphere(unsigned int sectors, unsigned int stacks)
    : VAO(0), VBO(0), EBO(0), indexCount(0)
{
    buildMesh(sectors, stacks);
}

Sphere::~Sphere() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::buildMesh(unsigned int sectors, unsigned int stacks) {
    // ── Generate Vertices ──────────────────────────────────
    // Each vertex has: position (3), normal (3), texcoord (2) = 8 floats
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float sectorStep = 2.0f * static_cast<float>(M_PI) / sectors;
    float stackStep  = static_cast<float>(M_PI) / stacks;

    for (unsigned int i = 0; i <= stacks; i++) {
        // Stack angle: from pi/2 (top) to -pi/2 (bottom)
        float stackAngle = static_cast<float>(M_PI) / 2.0f - i * stackStep;
        float xy = cosf(stackAngle);  // r * cos(phi)
        float z  = sinf(stackAngle);  // r * sin(phi)

        for (unsigned int j = 0; j <= sectors; j++) {
            // Sector angle: from 0 to 2*pi
            float sectorAngle = j * sectorStep;

            // Position (unit sphere, radius = 1)
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            // Normal (same as position for unit sphere)
            float nx = x;
            float ny = y;
            float nz = z;

            // Texture coordinates
            float s = static_cast<float>(j) / sectors;
            float t = static_cast<float>(i) / stacks;

            // Add vertex data: pos.x, pos.y, pos.z, norm.x, norm.y, norm.z, tex.s, tex.t
            vertices.push_back(x);
            vertices.push_back(z);   // Swap y and z for Y-up convention
            vertices.push_back(y);
            vertices.push_back(nx);
            vertices.push_back(nz);
            vertices.push_back(ny);
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // ── Generate Indices ───────────────────────────────────
    // Two triangles per quad, connecting adjacent stacks and sectors
    for (unsigned int i = 0; i < stacks; i++) {
        unsigned int k1 = i * (sectors + 1);       // Current stack
        unsigned int k2 = (i + 1) * (sectors + 1); // Next stack

        for (unsigned int j = 0; j < sectors; j++, k1++, k2++) {
            // Two triangles per sector (except at poles)
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    indexCount = static_cast<unsigned int>(indices.size());

    // ── Upload to GPU ──────────────────────────────────────
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    // Index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // Vertex attributes (stride = 8 floats per vertex)
    int stride = 8 * sizeof(float);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}
