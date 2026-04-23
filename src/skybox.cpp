// ============================================================
// Skybox Implementation
// Renders a star-field background using an inverted sphere
// ============================================================

#include "skybox.h"
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Skybox::Skybox() : textureID(0), VAO(0), VBO(0), EBO(0), indexCount(0), loaded(false) {
    buildSkyboxSphere();
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    if (textureID) glDeleteTextures(1, &textureID);
}

bool Skybox::loadTexture(const std::string& path) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "[Skybox] Failed to load: " << path << ", generating procedural.\n";
        generateProceduralTexture();
        return false;
    }
    GLenum fmt = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    loaded = true;
    std::cout << "[Skybox] Loaded (" << width << "x" << height << ")\n";
    return true;
}

void Skybox::generateProceduralTexture() {
    const int W = 2048, H = 1024;
    std::vector<unsigned char> px(W * H * 3, 0);
    std::srand(42);
    for (int i = 0; i < 8000; i++) {
        int x = std::rand() % W, y = std::rand() % H;
        int idx = (y * W + x) * 3;
        float b = 0.3f + 0.7f * (std::rand() % 1000) / 1000.0f;
        unsigned char v = (unsigned char)(255 * b);
        px[idx] = v; px[idx+1] = v; px[idx+2] = v;
    }
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    loaded = true;
    std::cout << "[Skybox] Procedural star field generated\n";
}

void Skybox::draw(Shader& shader, const Camera& camera, int sw, int sh) const {
    if (!loaded) return;
    glDepthFunc(GL_LEQUAL);
    shader.use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)sw/(float)sh, 0.1f, 1000.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.setInt("skyTexture", 0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::buildSkyboxSphere(unsigned int sec, unsigned int stk) {
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    float R = 500.0f;
    float secStep = 2.0f * (float)M_PI / sec;
    float stkStep = (float)M_PI / stk;
    for (unsigned i = 0; i <= stk; i++) {
        float sa = (float)M_PI / 2.0f - i * stkStep;
        float xy = R * cosf(sa), z = R * sinf(sa);
        for (unsigned j = 0; j <= sec; j++) {
            float ca = j * secStep;
            float x = xy * cosf(ca), y = xy * sinf(ca);
            verts.push_back(x); verts.push_back(z); verts.push_back(y);
            verts.push_back(0); verts.push_back(0); verts.push_back(0);
            verts.push_back((float)j/sec); verts.push_back((float)i/stk);
        }
    }
    for (unsigned i = 0; i < stk; i++) {
        unsigned k1 = i*(sec+1), k2 = (i+1)*(sec+1);
        for (unsigned j = 0; j < sec; j++, k1++, k2++) {
            if (i != 0) { idx.push_back(k1); idx.push_back(k1+1); idx.push_back(k2); }
            if (i != stk-1) { idx.push_back(k1+1); idx.push_back(k2+1); idx.push_back(k2); }
        }
    }
    indexCount = (unsigned)idx.size();
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned), idx.data(), GL_STATIC_DRAW);
    int s = 8*sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, s, (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, s, (void*)(3*sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, s, (void*)(6*sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}
