#pragma once
#include <vector>
#include "Matrix.h"
#include "Vector.h"

enum class SolutionType {
    None,     // решений нет
    Unique,   // одно решение
    Infinite  // бесконечно много
};

struct Solution {
    SolutionType type;
    Vector x;                    // решение (при Infinite - частное)
    size_t freeCount;            // сколько свободных переменных
    std::vector<size_t> freeCols;// номера свободных переменных (с 0)
    std::vector<Vector> basis;   // базисные векторы общего решения
};

// решает систему a * x = b методом Гаусса
Solution solveGauss(Matrix a, Vector b);