// ============================================================
// SolarSystem Class - Main simulation manager
// Creates planets, handles updates, rendering, and user controls
// ============================================================
#pragma once

#include "shader.h"
#include "camera.h"
#include "sphere.h"
#include "planet.h"
#include "skybox.h"
#include "orbit_renderer.h"

#include <vector>
#include <memory>

class SolarSystem {
public:
    SolarSystem();
    ~SolarSystem();

    /// Initialize all resources (shaders, meshes, planets, textures)
    bool init();

    /// Update simulation state
    void update(float deltaTime);

    /// Render the entire scene
    void render(Camera& camera, int screenWidth, int screenHeight);

    // ── User Controls ──────────────────────────────────────
    void toggleOrbits();
    void togglePause();
    void toggleAxes();
    void toggleGrid();
    void toggleTransformDemo();
    void adjustSpeed(float factor);
    void selectNextPlanet();
    void selectPreviousPlanet();
    void focusOnSelectedPlanet(Camera& camera);
    void printPlanetInfo() const;

    // State queries
    bool isPaused() const { return paused; }
    float getSpeedMultiplier() const { return speedMultiplier; }
    int getSelectedPlanetIndex() const { return selectedPlanet; }
    const Planet& getSelectedPlanet() const { return planets[selectedPlanet]; }

private:
    // ── Shaders ────────────────────────────────────────────
    std::unique_ptr<Shader> planetShader;
    std::unique_ptr<Shader> ringShader;
    std::unique_ptr<Shader> sunShader;
    std::unique_ptr<Shader> lineShader;
    std::unique_ptr<Shader> skyboxShader;

    // ── Geometry ───────────────────────────────────────────
    std::unique_ptr<Sphere> sphere;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<OrbitRenderer> orbitRenderer;

    // ── Planets ────────────────────────────────────────────
    std::vector<Planet> planets;
    Planet sun;

    // ── Simulation State ───────────────────────────────────
    float speedMultiplier;
    bool paused;
    bool showOrbits;
    bool showAxes;
    bool showGrid;
    bool showTransformDemo;
    int selectedPlanet;
    float elapsedTime;

    /// Create all planets with realistic parameters
    void createPlanets();

    /// Load all textures
    void loadTextures();

    /// Setup orbit visualization using midpoint circle algorithm
    void setupOrbits();
    void setupOrbitsRecursive(Planet& planet, const glm::vec3& color);

    /// Render the transformation demo (reflection, shear)
    void renderTransformDemo(Camera& camera, int screenWidth, int screenHeight);

    /// Render orbits recursively
    void renderOrbits(const Planet& planet, const glm::mat4& parentOrbitMatrix);
};
