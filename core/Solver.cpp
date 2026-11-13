#include "Solver.h"
#include "MathError.h"
#include <utility>

Solution solveGauss(Matrix a, Vector b) {
    if (a.rows != b.size) {
        throw MathError("Число уравнений не совпадает с размером правой части");
    }

    size_t m = a.rows;     // уравнений
    size_t n = a.columns;  // неизвестных

    // расширенная матрица: коэффициенты и справа столбец b
    Matrix aug(m, n + 1);
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            aug.data[i][j] = a.data[i][j];
        }
        aug.data[i][n] = b.data[i];
    }

    std::vector<size_t> pivots; // в каком столбце опорный элемент каждой строки
    size_t row = 0;

    // прямой ход
    for (size_t col = 0; col < n && row < m; col++) {
        // первая строка с ненулевым элементом
        size_t pivot = row;
        while (pivot < m && aug.data[pivot][col] == 0) {
            pivot++;
        }
        if (pivot == m) {
            continue; // в столбце нет опорного - это свободная переменная
        }

        std::swap(aug.data[row], aug.data[pivot]);

        for (size_t j = row + 1; j < m; j++) {
            Fraction factor = aug.data[j][col] / aug.data[row][col];
            for (size_t k = col; k <= n; k++) {
                aug.data[j][k] -= aug.data[row][k] * factor;
            }
        }

        pivots.push_back(col);
        row++;
    }

    // строка 0 0 0 | не ноль - решений нет
    for (size_t i = row; i < m; i++) {
        if (aug.data[i][n] != 0) {
            return Solution{SolutionType::None, Vector(n), 0, {}, {}};
        }
    }

    // обратный ход: свободные переменные в x уже выставлены заранее.
    // homogeneous = true - правая часть считается нулевой (для базисных векторов)
    auto backSubstitute = [&](Vector& x, bool homogeneous) {
        for (size_t i = row; i-- > 0; ) {
            size_t col = pivots[i];
            Fraction sum = 0;
            if (!homogeneous) {
                sum = aug.data[i][n];
            }
            for (size_t j = col + 1; j < n; j++) {
                sum -= aug.data[i][j] * x.data[j];
            }
            x.data[col] = sum / aug.data[i][col];
        }
    };

    // частное решение: свободные переменные = 0
    Vector x(n);
    backSubstitute(x, false);

    if (row == n) {
        return Solution{SolutionType::Unique, x, 0, {}, {}};
    }

    // свободные столбцы - те, которые не стали опорными
    std::vector<bool> isPivot(n, false);
    for (size_t i = 0; i < pivots.size(); i++) {
        isPivot[pivots[i]] = true;
    }

    std::vector<size_t> freeCols;
    std::vector<Vector> basis;
    for (size_t f = 0; f < n; f++) {
        if (isPivot[f]) continue;

        Vector v(n);
        v.data[f] = 1; // эта свободная = 1, остальные свободные = 0
        backSubstitute(v, true);

        freeCols.push_back(f);
        basis.push_back(v);
    }

    return Solution{SolutionType::Infinite, x, n - row, freeCols, basis};
}