// ============================================================
// Midpoint Circle Algorithm
// Manually implemented (NOT using OpenGL built-in primitives)
// Used for: generating planetary orbit paths
// ============================================================
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <utility>

namespace algorithms {

/// Generate points on a circle using the Midpoint Circle Algorithm
/// @param centerX, centerY  Center of circle
/// @param radius  Radius in pixel/unit coordinates
/// @return Vector of (x, y) points forming the circle outline
std::vector<std::pair<int, int>> midpoint_circle(int centerX, int centerY, int radius);

/// 3D variant: Generate orbit points in the XZ plane
/// Uses the midpoint circle algorithm internally
/// @param radius  Orbit radius in world units
/// @param resolution  Internal resolution (higher = more points)
/// @return Vector of 3D points (x, 0, z) forming the orbit path
std::vector<glm::vec3> midpoint_circle_orbit(float radius, int resolution = 1000);

} // namespace algorithms
