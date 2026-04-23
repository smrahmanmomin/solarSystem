// ============================================================
// DDA Line Algorithm - Digital Differential Analyzer
// Manually implemented (NOT using OpenGL built-in primitives)
// Used for: axis lines, debug lines
// ============================================================
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <utility>

namespace algorithms {

/// Generate points along a line using the DDA algorithm
/// @param x1, y1  Start point
/// @param x2, y2  End point
/// @return Vector of (x, y) points along the line
std::vector<std::pair<float, float>> dda_line(float x1, float y1, float x2, float y2);

/// 3D variant: generate points along a line in 3D space
/// Maps the DDA 2D output to a 3D line on a given plane
std::vector<glm::vec3> dda_line_3d(const glm::vec3& start, const glm::vec3& end, int numPoints = 100);

} // namespace algorithms
