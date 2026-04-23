// ============================================================
// Main Entry Point - 3D Solar System Simulation
// Sets up GLFW window, OpenGL context, and runs the main loop
// ============================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "solar_system.h"
#include "camera.h"

// ── Window settings ────────────────────────────────────────────
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

// ── Global state (minimal, as required by GLFW callbacks) ──────
Camera camera(glm::vec3(0.0f, 30.0f, 80.0f));
SolarSystem solarSystem;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int screenWidth = SCR_WIDTH;
int screenHeight = SCR_HEIGHT;

// ── Callback declarations ──────────────────────────────────────
void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void scroll_callback(GLFWwindow* w, double xoff, double yoff);
void key_callback(GLFWwindow* w, int key, int sc, int action, int mods);
void processInput(GLFWwindow* w);

// ================================================================
// MAIN
// ================================================================
int main() {
    // ── 1. Initialize GLFW ─────────────────────────────────
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    // ── 2. Create Window ───────────────────────────────────
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
        "Solar System Simulation | OpenGL 3.3", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ── 3. Initialize GLAD ─────────────────────────────────
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // ── 4. OpenGL Configuration ────────────────────────────
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(2.0f);

    // ── 5. Initialize Solar System ─────────────────────────
    if (!solarSystem.init()) {
        std::cerr << "Failed to initialize Solar System\n";
        glfwTerminate();
        return -1;
    }

    // ── 6. Main Render Loop ────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        // Delta time calculation
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Update simulation
        solarSystem.update(deltaTime);

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        solarSystem.render(camera, screenWidth, screenHeight);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ── 7. Cleanup ─────────────────────────────────────────
    glfwTerminate();
    return 0;
}

// ================================================================
// Input Processing
// ================================================================
void processInput(GLFWwindow* w) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);

    // Camera movement
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(CAM_FORWARD, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(CAM_BACKWARD, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(CAM_LEFT, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(CAM_RIGHT, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(CAM_UP, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(CAM_DOWN, deltaTime);

    // Speed control (hold)
    if (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = DEFAULT_SPEED * 3.0f;
    else
        camera.MovementSpeed = DEFAULT_SPEED;
}

// ── Key callback (single press actions) ────────────────────────
void key_callback(GLFWwindow* w, int key, int sc, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_O: solarSystem.toggleOrbits(); break;
        case GLFW_KEY_P: solarSystem.togglePause(); break;
        case GLFW_KEY_X: solarSystem.toggleAxes(); break;
        case GLFW_KEY_G: solarSystem.toggleGrid(); break;
        case GLFW_KEY_T: solarSystem.toggleTransformDemo(); break;
        case GLFW_KEY_EQUAL:      solarSystem.adjustSpeed(1.5f); break; // +
        case GLFW_KEY_MINUS:      solarSystem.adjustSpeed(0.67f); break; // -
        case GLFW_KEY_RIGHT_BRACKET: solarSystem.selectNextPlanet(); break;
        case GLFW_KEY_LEFT_BRACKET:  solarSystem.selectPreviousPlanet(); break;
        case GLFW_KEY_F: solarSystem.focusOnSelectedPlanet(camera); break;
        case GLFW_KEY_I: solarSystem.printPlanetInfo(); break;
    }
}

// ── GLFW Callbacks ─────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

void mouse_callback(GLFWwindow*, double xposIn, double yposIn) {
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed: y goes bottom-to-top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}
