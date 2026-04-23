// ============================================================
// Camera Class - FPS-style free-look camera
// Supports keyboard movement, mouse look, and scroll zoom
// ============================================================
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/// Camera movement directions
enum CameraMovement {
    CAM_FORWARD,
    CAM_BACKWARD,
    CAM_LEFT,
    CAM_RIGHT,
    CAM_UP,
    CAM_DOWN
};

// Default camera values
constexpr float DEFAULT_YAW         = -90.0f;
constexpr float DEFAULT_PITCH       =  0.0f;
constexpr float DEFAULT_SPEED       = 25.0f;
constexpr float DEFAULT_SENSITIVITY = 0.1f;
constexpr float DEFAULT_ZOOM        = 45.0f;

class Camera {
public:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    /// Construct camera at given position
    Camera(glm::vec3 position = glm::vec3(0.0f, 10.0f, 40.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);

    /// Get the view matrix using Euler angles and the LookAt matrix
    glm::mat4 GetViewMatrix() const;

    /// Process keyboard input for movement
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    /// Process mouse movement for looking around
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    /// Process mouse scroll for zoom
    void ProcessMouseScroll(float yoffset);

    /// Smoothly move camera to look at a target position
    void LookAt(const glm::vec3& target);

private:
    /// Recalculate Front, Right, Up vectors from Euler angles
    void updateCameraVectors();
};
