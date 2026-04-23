// ============================================================
// SolarSystem Implementation
// Main simulation manager - creates planets, handles updates
// ============================================================

#include "solar_system.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

SolarSystem::SolarSystem()
    : sun("Sun", 5.0f, 0.0f, 2.0f, 0.0f, 0.0f, glm::vec3(1.0f, 0.8f, 0.3f))
    , speedMultiplier(1.0f)
    , paused(false)
    , showOrbits(true)
    , showAxes(false)
    , showGrid(false)
    , showTransformDemo(false)
    , selectedPlanet(2) // Earth by default
    , elapsedTime(0.0f)
{}

SolarSystem::~SolarSystem() {}

bool SolarSystem::init() {
    std::cout << "========================================\n";
    std::cout << "  3D Solar System Simulation\n";
    std::cout << "  Modern OpenGL | Phong Lighting\n";
    std::cout << "========================================\n\n";

    // Load shaders
    try {
        planetShader = std::make_unique<Shader>("shaders/planet.vert", "shaders/planet.frag");
        ringShader   = std::make_unique<Shader>("shaders/ring.vert", "shaders/ring.frag");
        sunShader    = std::make_unique<Shader>("shaders/sun.vert", "shaders/sun.frag");
        lineShader   = std::make_unique<Shader>("shaders/line.vert", "shaders/line.frag");
        skyboxShader = std::make_unique<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
    } catch (...) {
        std::cerr << "[SolarSystem] Failed to load shaders!\n";
        return false;
    }
    std::cout << "[Init] Shaders loaded.\n";

    // Create sphere mesh
    sphere = std::make_unique<Sphere>(72, 36);
    std::cout << "[Init] Sphere mesh created.\n";

    // Create skybox
    skybox = std::make_unique<Skybox>();
    skybox->loadTexture("textures/stars.jpg");

    // Create orbit renderer
    orbitRenderer = std::make_unique<OrbitRenderer>();

    // Create planets
    createPlanets();
    loadTextures();
    setupOrbits();

    // Generate axes and grid using DDA and Bresenham algorithms
    orbitRenderer->generateAxes(100.0f);
    orbitRenderer->generateGrid(200.0f, 10.0f);

    std::cout << "\n[Init] Solar System ready!\n";
    std::cout << "========================================\n";
    std::cout << " Controls:\n";
    std::cout << "  WASD    - Move camera\n";
    std::cout << "  QE      - Move up/down\n";
    std::cout << "  Mouse   - Look around\n";
    std::cout << "  Scroll  - Zoom\n";
    std::cout << "  O       - Toggle orbits\n";
    std::cout << "  P       - Pause/Resume\n";
    std::cout << "  +/-     - Speed up/down\n";
    std::cout << "  [/]     - Select planet\n";
    std::cout << "  F       - Focus on planet\n";
    std::cout << "  G       - Toggle grid\n";
    std::cout << "  X       - Toggle axes\n";
    std::cout << "  T       - Transform demo\n";
    std::cout << "  I       - Planet info\n";
    std::cout << "========================================\n\n";

    return true;
}

void SolarSystem::createPlanets() {
    //                    Name       Radius  Orbit   RotSpd  RevSpd  Tilt   Color
    planets.emplace_back("Mercury",  0.4f,   10.0f,  10.0f,  47.0f,  0.03f, glm::vec3(0.7f, 0.7f, 0.7f));
    planets.emplace_back("Venus",    0.9f,   15.0f,   3.0f,  35.0f,  177.4f, glm::vec3(0.9f, 0.7f, 0.3f));
    planets.emplace_back("Earth",    1.0f,   22.0f,  30.0f,  29.0f,  23.4f, glm::vec3(0.2f, 0.4f, 0.8f));
    planets.emplace_back("Mars",     0.5f,   30.0f,  28.0f,  24.0f,  25.2f, glm::vec3(0.8f, 0.3f, 0.2f));
    planets.emplace_back("Jupiter",  3.0f,   45.0f,  45.0f,  13.0f,  3.1f,  glm::vec3(0.8f, 0.6f, 0.4f));
    planets.emplace_back("Saturn",   2.5f,   60.0f,  38.0f,   9.0f,  26.7f, glm::vec3(0.98f, 0.95f, 0.75f)); // Pastel yellow
    planets.emplace_back("Uranus",   1.8f,   75.0f,  20.0f,   6.0f,  97.8f, glm::vec3(0.5f, 0.8f, 0.9f));
    planets.emplace_back("Neptune",  1.7f,   90.0f,  22.0f,   5.0f,  28.3f, glm::vec3(0.3f, 0.4f, 0.9f));

    planets[5].hasRing = true;
    planets[5].ringInnerRadius = 3.2f;
    planets[5].ringOuterRadius = 7.5f; // Massive ring
    planets[5].oblateness = 0.9f; // Flattened at poles

    // --- Moons ---
    // Earth: Moon
    planets[2].addMoon(Planet("Moon", 0.27f, 2.5f, 10.0f, 60.0f, 1.5f, glm::vec3(0.6f)));

    // Mars: Phobos, Deimos
    planets[3].addMoon(Planet("Phobos", 0.15f, 1.2f, 0.0f, 90.0f, 0.0f, glm::vec3(0.5f)));
    planets[3].addMoon(Planet("Deimos", 0.1f, 1.8f, 0.0f, 70.0f, 0.0f, glm::vec3(0.6f)));

    // Jupiter: Io, Europa, Ganymede, Callisto
    planets[4].addMoon(Planet("Io",       0.3f, 4.0f, 0.0f, 150.0f, 0.0f, glm::vec3(0.8f, 0.8f, 0.2f)));
    planets[4].addMoon(Planet("Europa",   0.28f, 5.0f, 0.0f, 100.0f, 0.0f, glm::vec3(0.7f, 0.7f, 0.7f)));
    planets[4].addMoon(Planet("Ganymede", 0.4f, 6.5f, 0.0f, 70.0f, 0.0f, glm::vec3(0.5f, 0.5f, 0.4f)));
    planets[4].addMoon(Planet("Callisto", 0.38f, 8.0f, 0.0f, 40.0f, 0.0f, glm::vec3(0.4f, 0.4f, 0.4f)));

    // Saturn: Titan, Enceladus
    planets[5].addMoon(Planet("Titan",     0.35f, 7.5f, 0.0f, 50.0f, 0.0f, glm::vec3(0.8f, 0.6f, 0.2f)));
    planets[5].addMoon(Planet("Enceladus", 0.15f, 6.5f, 0.0f, 80.0f, 0.0f, glm::vec3(0.9f, 0.9f, 0.9f)));

    // Uranus: Titania, Oberon
    planets[6].addMoon(Planet("Titania", 0.2f, 3.0f, 0.0f, 60.0f, 0.0f, glm::vec3(0.6f)));
    planets[6].addMoon(Planet("Oberon",  0.18f, 4.0f, 0.0f, 45.0f, 0.0f, glm::vec3(0.5f)));

    // Neptune: Triton
    planets[7].addMoon(Planet("Triton", 0.22f, 3.5f, 0.0f, -50.0f, 0.0f, glm::vec3(0.7f, 0.8f, 0.8f))); // Retrograde

    std::cout << "[Init] Created planets and moons.\n";
}

void SolarSystem::loadTextures() {
    sun.loadTexture("textures/sun.jpg");

    std::string names[] = {"mercury","venus","earth","mars","jupiter","saturn","uranus","neptune"};
    for (size_t i = 0; i < planets.size() && i < 8; i++) {
        planets[i].loadTexture("textures/" + names[i] + ".jpg");
    }

    // Load Saturn's ring
    planets[5].loadRingTexture("textures/saturn_ring.png");
    planets[5].initRing();
}

void SolarSystem::setupOrbitsRecursive(Planet& planet, const glm::vec3& color) {
    planet.orbitId = orbitRenderer->addOrbit(planet.orbitRadius, color);
    for (auto& moon : planet.moons) {
        setupOrbitsRecursive(moon, glm::vec3(0.3f, 0.3f, 0.3f)); // Dimmer color for moon orbits
    }
}

void SolarSystem::setupOrbits() {
    // Generate orbit paths using MIDPOINT CIRCLE ALGORITHM
    glm::vec3 orbitColors[] = {
        {0.6f, 0.6f, 0.6f}, {0.9f, 0.7f, 0.3f}, {0.3f, 0.5f, 0.9f}, {0.9f, 0.4f, 0.3f},
        {0.8f, 0.6f, 0.4f}, {0.9f, 0.8f, 0.5f}, {0.5f, 0.8f, 0.9f}, {0.4f, 0.5f, 0.9f}
    };
    for (size_t i = 0; i < planets.size(); i++) {
        setupOrbitsRecursive(planets[i], orbitColors[i]);
    }
    std::cout << "[Init] Orbits generated (Midpoint Circle Algorithm).\n";
}

void SolarSystem::update(float deltaTime) {
    if (paused) return;
    elapsedTime += deltaTime;
    sun.update(deltaTime, speedMultiplier);
    for (auto& p : planets) p.update(deltaTime, speedMultiplier);
}

void SolarSystem::render(Camera& camera, int sw, int sh) {
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)sw/(float)sh, 0.1f, 2000.0f);

    // 1. Skybox
    skybox->draw(*skyboxShader, camera, sw, sh);

    // 2. Sun (emissive, no lighting)
    sunShader->use();
    sunShader->setMat4("view", view);
    sunShader->setMat4("projection", proj);
    sunShader->setFloat("time", elapsedTime);
    sunShader->setFloat("glowIntensity", 1.5f);
    glm::mat4 sunModel(1.0f);
    sunModel = glm::rotate(sunModel, glm::radians(sun.currentRotation), glm::vec3(0,1,0));
    sunModel = glm::scale(sunModel, glm::vec3(sun.radius));
    sunShader->setMat4("model", sunModel);
    if (sun.hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun.textureID);
        sunShader->setInt("sunTexture", 0);
    }
    sphere->draw();

    // 3. Planets (with Phong lighting)
    ringShader->use();
    ringShader->setMat4("view", view);
    ringShader->setMat4("projection", proj);
    ringShader->setVec3("lightPos", glm::vec3(0.0f));
    ringShader->setVec3("viewPos", camera.Position);
    ringShader->setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.8f));
    ringShader->setFloat("time", elapsedTime);

    planetShader->use();
    planetShader->setMat4("view", view);
    planetShader->setMat4("projection", proj);
    planetShader->setVec3("lightPos", glm::vec3(0.0f));
    planetShader->setVec3("viewPos", camera.Position);
    planetShader->setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.8f));
    planetShader->setFloat("ambientStrength", 0.15f);
    planetShader->setFloat("specularStrength", 0.5f);
    planetShader->setFloat("shininess", 32.0f);
    planetShader->setInt("planetTexture", 0);
    for (auto& p : planets) p.draw(*sphere, *planetShader, ringShader.get());

    // 4. Orbits
    if (showOrbits) {
        lineShader->use();
        lineShader->setMat4("view", view);
        lineShader->setMat4("projection", proj);
        for (auto& p : planets) {
            renderOrbits(p, glm::mat4(1.0f));
        }
    }

    // 5. Axes
    if (showAxes) {
        lineShader->use();
        lineShader->setMat4("view", view);
        lineShader->setMat4("projection", proj);
        orbitRenderer->drawAxes(*lineShader);
    }

    // 6. Grid
    if (showGrid) {
        lineShader->use();
        lineShader->setMat4("view", view);
        lineShader->setMat4("projection", proj);
        orbitRenderer->drawGrid(*lineShader);
    }

    // 7. Transform demo
    if (showTransformDemo) renderTransformDemo(camera, sw, sh);
}

void SolarSystem::renderOrbits(const Planet& planet, const glm::mat4& parentOrbitMatrix) {
    orbitRenderer->drawOrbit(planet.orbitId, *lineShader, parentOrbitMatrix);
    glm::mat4 currentOrbitMatrix = planet.getOrbitMatrix(parentOrbitMatrix);
    for (const auto& moon : planet.moons) {
        renderOrbits(moon, currentOrbitMatrix);
    }
}

void SolarSystem::renderTransformDemo(Camera& camera, int sw, int sh) {
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)sw/(float)sh, 0.1f, 2000.0f);

    planetShader->use();
    planetShader->setMat4("view", view);
    planetShader->setMat4("projection", proj);

    if (selectedPlanet >= 0 && selectedPlanet < (int)planets.size()) {
        auto& p = planets[selectedPlanet];

        // REFLECTION: mirror across XZ plane (Y=0)
        glm::mat4 reflected = p.getReflectedModelMatrix(glm::vec3(0,1,0));
        planetShader->setMat4("model", reflected);
        planetShader->setFloat("ambientStrength", 0.3f);
        if (p.hasTexture) { glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, p.textureID); }
        sphere->draw();

        // SHEAR: distorted version
        glm::mat4 sheared = p.getShearedModelMatrix(0.5f);
        planetShader->setMat4("model", sheared);
        planetShader->setFloat("ambientStrength", 0.25f);
        sphere->draw();
    }
}

// ── User Controls ──────────────────────────────────────────────

void SolarSystem::toggleOrbits() { showOrbits = !showOrbits; std::cout << "Orbits: " << (showOrbits?"ON":"OFF") << "\n"; }
void SolarSystem::togglePause()  { paused = !paused; std::cout << (paused?"PAUSED":"RESUMED") << "\n"; }
void SolarSystem::toggleAxes()   { showAxes = !showAxes; std::cout << "Axes: " << (showAxes?"ON":"OFF") << "\n"; }
void SolarSystem::toggleGrid()   { showGrid = !showGrid; std::cout << "Grid: " << (showGrid?"ON":"OFF") << "\n"; }
void SolarSystem::toggleTransformDemo() { showTransformDemo = !showTransformDemo; std::cout << "Transform Demo: " << (showTransformDemo?"ON":"OFF") << "\n"; }

void SolarSystem::adjustSpeed(float factor) {
    speedMultiplier *= factor;
    speedMultiplier = glm::clamp(speedMultiplier, 0.01f, 100.0f);
    std::cout << "Speed: " << speedMultiplier << "x\n";
}

void SolarSystem::selectNextPlanet() {
    selectedPlanet = (selectedPlanet + 1) % (int)planets.size();
    std::cout << "Selected: " << planets[selectedPlanet].name << "\n";
}

void SolarSystem::selectPreviousPlanet() {
    selectedPlanet = (selectedPlanet - 1 + (int)planets.size()) % (int)planets.size();
    std::cout << "Selected: " << planets[selectedPlanet].name << "\n";
}

void SolarSystem::focusOnSelectedPlanet(Camera& camera) {
    if (selectedPlanet < 0 || selectedPlanet >= (int)planets.size()) return;
    auto& p = planets[selectedPlanet];
    glm::vec3 pos = p.getPosition();
    camera.Position = pos + glm::vec3(0, p.radius*3, p.radius*5);
    camera.LookAt(pos);
    std::cout << "Focused on: " << p.name << "\n";
}

void SolarSystem::printPlanetInfo() const {
    if (selectedPlanet < 0 || selectedPlanet >= (int)planets.size()) return;
    auto& p = planets[selectedPlanet];
    std::cout << "\n--- " << p.name << " ---\n";
    std::cout << "  Radius:     " << p.radius << "\n";
    std::cout << "  Orbit:      " << p.orbitRadius << "\n";
    std::cout << "  Rot Speed:  " << p.rotationSpeed << " deg/s\n";
    std::cout << "  Rev Speed:  " << p.revolutionSpeed << " deg/s\n";
    std::cout << "  Axial Tilt: " << p.axialTilt << " deg\n";
    std::cout << "  Position:   (" << p.getPosition().x << ", " << p.getPosition().z << ")\n\n";
}
