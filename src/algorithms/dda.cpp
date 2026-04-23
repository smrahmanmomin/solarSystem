// ============================================================
// DDA Line Algorithm Implementation
// Digital Differential Analyzer - generates points along a line
// 
// HOW IT WORKS:
// 1. Calculate dx = x2-x1 and dy = y2-y1
// 2. Find the number of steps = max(|dx|, |dy|)
// 3. Calculate x_increment = dx/steps, y_increment = dy/steps
// 4. Starting from (x1,y1), add increments for each step
//
// USAGE IN PROJECT:
// - Generate 3D axis lines (X, Y, Z axes)
// - Generate debug visualization lines
// ============================================================

#include "algorithms/dda.h"
#include <cmath>
#include <algorithm>

namespace algorithms {

std::vector<std::pair<float, float>> dda_line(float x1, float y1, float x2, float y2) {
    std::vector<std::pair<float, float>> points;

    // Step 1: Calculate differences
    float dx = x2 - x1;
    float dy = y2 - y1;

    // Step 2: Determine number of steps
    // We use the larger of |dx| or |dy| to ensure no gaps
    int steps = static_cast<int>(std::max(std::abs(dx), std::abs(dy)));

    if (steps == 0) {
        // Start and end are the same point
        points.push_back({x1, y1});
        return points;
    }

    // Step 3: Calculate increment per step
    float xIncrement = dx / static_cast<float>(steps);
    float yIncrement = dy / static_cast<float>(steps);

    // Step 4: Generate points along the line
    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        // Round to nearest integer position (as per DDA algorithm)
        points.push_back({std::round(x), std::round(y)});
        x += xIncrement;
        y += yIncrement;
    }

    return points;
}

std::vector<glm::vec3> dda_line_3d(const glm::vec3& start, const glm::vec3& end, int numPoints) {
    std::vector<glm::vec3> points;

    // Use DDA concept in 3D: generate evenly spaced points
    // Project the 3D line into 2D (XZ plane) for the DDA algorithm
    auto ddaPoints = dda_line(
        start.x * numPoints, start.z * numPoints,
        end.x * numPoints, end.z * numPoints
    );

    if (ddaPoints.empty()) {
        points.push_back(start);
        return points;
    }

    // Map DDA points back to 3D, interpolating Y
    float scale = 1.0f / numPoints;
    for (size_t i = 0; i < ddaPoints.size(); i++) {
        float t = static_cast<float>(i) / static_cast<float>(ddaPoints.size() - 1);
        float y = start.y + t * (end.y - start.y);
        points.push_back(glm::vec3(
            ddaPoints[i].first * scale,
            y,
            ddaPoints[i].second * scale
        ));
    }

    return points;
}

} // namespace algorithms
