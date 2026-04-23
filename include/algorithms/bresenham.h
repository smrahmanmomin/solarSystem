// ============================================================
// Bresenham Line Algorithm
// Manually implemented (NOT using OpenGL built-in primitives)
// Used for: grid lines, debug visualization
// ============================================================
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <utility>

namespace algorithms {

/// Generate points along a line using Bresenham's algorithm
/// @param x1, y1  Start point (integer coordinates)
/// @param x2, y2  End point (integer coordinates)
/// @return Vector of (x, y) integer points along the line
std::vector<std::pair<int, int>> bresenham_line(int x1, int y1, int x2, int y2);

/// 3D variant: generate points along a line in 3D space
/// Uses Bresenham internally, maps to 3D coordinates
std::vector<glm::vec3> bresenham_line_3d(const glm::vec3& start, const glm::vec3& end, float scale = 1.0f);

} // namespace algorithms
