#include "Matrix.h"
#include "MathError.h"
#include <sstream>
#include <cmath>

// Конструктор
Matrix::Matrix(size_t r, size_t c) {
    if (r == 0 or c == 0) {
        throw MathError("Размер матрицы должен быть больше нуля");
    }
    rows = r;
    columns = c;

    // r строк, в каждой c нулей
    data = std::vector<std::vector<double>>(r, std::vector<double>(c, 0));
}

//сложение
Matrix Matrix::add(Matrix other) {
    if (rows != other.rows or columns != other.columns) {
        throw MathError("Нельзя сложить: разные размеры");
    }

    Matrix result(rows, columns);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }
    return result;
}

//вычитание
Matrix Matrix::subtract(Matrix other) {
    if (rows != other.rows or columns != other.columns) {
        throw MathError("Нельзя вычесть: разные размеры");
    }

    Matrix result(rows, columns);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }
    return result;
}

//вывод матрицы строкой
std::string Matrix::toString() {
    std::ostringstream out;

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            out << data[i][j];

            if (j + 1 < columns) {
                out << " "; //не писать пробел у последнего числа
            }
        }
        out << "\n";
    }
    return out.str();
}

//умножение на скаляр
Matrix Matrix::multiplyByNumber(double number) {
    Matrix result(rows, columns);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            result.data[i][j] = data[i][j] * number;
        }
    }
    return result;
}

//умножение на матрицу
Matrix Matrix::multiply(Matrix other) {
    if (columns != other.rows) {
        throw MathError("Количество столбцов у 1 матрицы не совпадает с количеством строк у 2 матрицы");
    }

    Matrix result(rows, other.columns);
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < other.columns; j++) {
            for (size_t k = 0; k < columns; k++) {
                result.data[i][j] += data[i][k] * other.data[k][j];
            }
        }
    }
    return result;
}

//транспонирование
Matrix Matrix::transpose() {
    Matrix result(columns, rows);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            result.data[j][i] = data[i][j];
        }
    }
    return result;
}

//определитель
double Matrix::determinant() {
    if (rows != columns) {
        throw MathError("Определитель можно найти только у квадратной матрицы");
    }

    Matrix temp = *this;
    double det = 1.0;

    for (size_t i = 0; i < rows; i++) {
        size_t maxElement = i;
        for (size_t j = i + 1; j < rows; j++) {
            if (std::abs(temp.data[j][i]) > std::abs(temp.data[maxElement][i])) {
                maxElement = j;
            }
        }

        if (std::abs(temp.data[maxElement][i]) < 1e-9) {return 0.0;}
        if (maxElement != i) {swap(temp.data[i], temp.data[maxElement]); det = -det; }
        det *= temp.data[i][i];

        for (size_t j = i + 1; j < rows; j++) {
            double factor = temp.data[j][i] / temp.data[i][i];
            for (size_t k = i; k < rows; k++) {
                temp.data[j][k] -= temp.data[i][k] * factor;
            }
        }
    }
    return det;
}