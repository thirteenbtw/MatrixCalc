#pragma once
#include <stdexcept>

// мат. невозможная операция
struct MathError : std::runtime_error {
    using std::runtime_error::runtime_error;
};