// ============================================================
// Midpoint Circle Algorithm Implementation
// Generates circle outline points using only integer arithmetic
//
// HOW IT WORKS:
// 1. Start at the top of the circle: (0, radius)
// 2. Calculate a decision parameter p = 1 - radius
// 3. For each step, decide whether to move E or SE based on p
// 4. Use 8-way symmetry: computing one octant gives all 8
//
// USAGE IN PROJECT:
// - Generate planetary orbit paths (the primary use case!)
// - Each planet's orbit is drawn using points from this algorithm
// ============================================================

#include "algorithms/midpoint_circle.h"
#include <cmath>
#include <algorithm>
#include <set>

namespace algorithms {

std::vector<std::pair<int, int>> midpoint_circle(int centerX, int centerY, int radius) {
    std::vector<std::pair<int, int>> points;

    // Start at the top of the circle
    int x = 0;
    int y = radius;

    // Initial decision parameter
    // p = 1 - r (derived from the midpoint criterion)
    int p = 1 - radius;

    // Helper lambda: add all 8 symmetric points
    // This is the key optimization of the midpoint circle algorithm
    auto plotCirclePoints = [&](int cx, int cy, int px, int py) {
        points.push_back({cx + px, cy + py});  // Octant 1
        points.push_back({cx - px, cy + py});  // Octant 2
        points.push_back({cx + px, cy - py});  // Octant 3
        points.push_back({cx - px, cy - py});  // Octant 4
        points.push_back({cx + py, cy + px});  // Octant 5
        points.push_back({cx - py, cy + px});  // Octant 6
        points.push_back({cx + py, cy - px});  // Octant 7
        points.push_back({cx - py, cy - px});  // Octant 8
    };

    // Plot initial points
    plotCirclePoints(centerX, centerY, x, y);

    // Iterate through the first octant (x < y)
    while (x < y) {
        x++;

        if (p < 0) {
            // Midpoint is inside the circle → move East
            // Next decision parameter
            p += 2 * x + 1;
        } else {
            // Midpoint is outside the circle → move South-East
            y--;
            p += 2 * (x - y) + 1;
        }

        // Plot all 8 symmetric points
        plotCirclePoints(centerX, centerY, x, y);
    }

    return points;
}

std::vector<glm::vec3> midpoint_circle_orbit(float radius, int resolution) {
    // Step 1: Generate circle points using midpoint algorithm
    // We use a large integer radius for precision
    auto circlePoints = midpoint_circle(0, 0, resolution);

    // Step 2: Convert to normalized float coordinates and remove duplicates
    // Use a set to track unique angle-sorted points
    struct AnglePoint {
        float angle;
        glm::vec3 pos;
        bool operator<(const AnglePoint& other) const { return angle < other.angle; }
    };
    std::set<AnglePoint> sortedPoints;

    float invRes = radius / static_cast<float>(resolution);

    for (auto& [px, py] : circlePoints) {
        float x = px * invRes;
        float z = py * invRes;
        float angle = std::atan2(z, x);
        sortedPoints.insert({angle, glm::vec3(x, 0.0f, z)});
    }

    // Step 3: Convert to vector, sorted by angle for smooth rendering
    std::vector<glm::vec3> result;
    result.reserve(sortedPoints.size());
    for (auto& ap : sortedPoints) {
        result.push_back(ap.pos);
    }

    return result;
}

} // namespace algorithms
