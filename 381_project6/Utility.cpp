#include "Utility.h"

// Tolerance used when comparing Points
constexpr double kPOINT_TOLERANCE = 1.0e-6;

bool double_tolerance_compare_eq(const double lhs, const double rhs) noexcept {
    return lhs < rhs + kPOINT_TOLERANCE && lhs > rhs - kPOINT_TOLERANCE;
}
