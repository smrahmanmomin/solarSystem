#include "ring.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ring::Ring(float innerRadius, float outerRadius, unsigned int segments) 
    : VAO(0), VBO(0), EBO(0), indexCount(0) {
    
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float angleStep = 2.0f * M_PI / segments;

    for (unsigned int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // Inner vertex
        vertices.push_back(innerRadius * cosA); // x
        vertices.push_back(0.0f);               // y
        vertices.push_back(innerRadius * sinA); // z
        vertices.push_back(0.0f);               // nx
        vertices.push_back(1.0f);               // ny
        vertices.push_back(0.0f);               // nz
        vertices.push_back(1.0f);               // texcoord (maps to bottom of texture, or vice versa depending on image)
        vertices.push_back(0.0f);

        // Outer vertex
        vertices.push_back(outerRadius * cosA); // x
        vertices.push_back(0.0f);               // y
        vertices.push_back(outerRadius * sinA); // z
        vertices.push_back(0.0f);               // nx
        vertices.push_back(1.0f);               // ny
        vertices.push_back(0.0f);               // nz
        vertices.push_back(1.0f);               // texcoord
        vertices.push_back(1.0f);
    }

    // Indices for triangle strip
    for (unsigned int i = 0; i < segments; ++i) {
        unsigned int inner1 = i * 2;
        unsigned int outer1 = i * 2 + 1;
        unsigned int inner2 = (i + 1) * 2;
        unsigned int outer2 = (i + 1) * 2 + 1;

        indices.push_back(inner1);
        indices.push_back(outer1);
        indices.push_back(inner2);

        indices.push_back(outer1);
        indices.push_back(outer2);
        indices.push_back(inner2);
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Ring::~Ring() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Ring::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
