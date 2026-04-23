// ============================================================
// Shader Class - Loads, compiles, and links GLSL shaders
// Provides uniform setter utilities
// ============================================================
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int ID;  // OpenGL program ID

    /// Construct shader from vertex and fragment shader file paths
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    /// Activate this shader program
    void use() const;

    // ── Uniform Setters ────────────────────────────────────
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    /// Check compilation/linking errors
    void checkCompileErrors(unsigned int shader, const std::string& type);
};
