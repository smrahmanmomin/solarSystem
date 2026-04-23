// ============================================================
// Bresenham Line Algorithm Implementation
// Integer-only line drawing algorithm (no floating point!)
//
// HOW IT WORKS:
// 1. Calculate |dx| and |dy|
// 2. Determine the "driving axis" (the one with larger delta)
// 3. Use a decision parameter to decide when to step on the other axis
// 4. Only uses integer addition and comparison - very efficient!
//
// USAGE IN PROJECT:
// - Generate grid lines on the XZ plane
// - Generate debug visualization lines
// ============================================================

#include "algorithms/bresenham.h"
#include <cmath>
#include <algorithm>

namespace algorithms {

std::vector<std::pair<int, int>> bresenham_line(int x1, int y1, int x2, int y2) {
    std::vector<std::pair<int, int>> points;

    // Step 1: Calculate absolute differences and directions
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;  // Step direction for x
    int sy = (y1 < y2) ? 1 : -1;  // Step direction for y

    // Step 2: Initialize the decision parameter
    // Bresenham's key insight: use error accumulation instead of division
    int err = dx - dy;

    int x = x1, y = y1;

    while (true) {
        // Record current point
        points.push_back({x, y});

        // Check if we've reached the end
        if (x == x2 && y == y2) break;

        // Step 3: Calculate error and decide which axis to step
        int e2 = 2 * err;

        // If error indicates we should step in x
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        // If error indicates we should step in y
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }

    return points;
}

std::vector<glm::vec3> bresenham_line_3d(const glm::vec3& start, const glm::vec3& end, float scale) {
    std::vector<glm::vec3> points;

    // Scale world coordinates to integer grid for Bresenham
    int scaleFactor = 100;  // Resolution of the integer grid
    int x1 = static_cast<int>(start.x * scaleFactor);
    int y1 = static_cast<int>(start.z * scaleFactor);
    int x2 = static_cast<int>(end.x * scaleFactor);
    int y2 = static_cast<int>(end.z * scaleFactor);

    // Run Bresenham on the integer grid
    auto linePoints = bresenham_line(x1, y1, x2, y2);

    // Convert back to 3D world coordinates
    float invScale = scale / static_cast<float>(scaleFactor);
    for (size_t i = 0; i < linePoints.size(); i++) {
        float t = linePoints.size() > 1
            ? static_cast<float>(i) / static_cast<float>(linePoints.size() - 1)
            : 0.0f;
        float y = start.y + t * (end.y - start.y);
        points.push_back(glm::vec3(
            linePoints[i].first * invScale,
            y,
            linePoints[i].second * invScale
        ));
    }

    return points;
}

} // namespace algorithms
