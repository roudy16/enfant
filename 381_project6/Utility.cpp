#include "Utility.h"

bool double_tolerance_compare_eq(const double lhs, const double rhs) {
    return lhs < rhs + kPOINT_TOLERANCE && lhs > rhs - kPOINT_TOLERANCE;
}
