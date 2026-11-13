#include "Matrix.h"
#include "MathError.h"
#include <sstream>
#include <utility>

// Конструктор
Matrix::Matrix(size_t r, size_t c) {
    if (r == 0 or c == 0) {
        throw MathError("Размер матрицы должен быть больше нуля");
    }
    rows = r;
    columns = c;

    // r строк, в каждой c нулей
    data = std::vector<std::vector<Fraction>>(r, std::vector<Fraction>(c, 0));
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
Matrix Matrix::multiplyByNumber(Fraction number) {
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
Fraction Matrix::determinant() {
    if (rows != columns) {
        throw MathError("Определитель можно найти только у квадратной матрицы");
    }

    Matrix temp = *this;
    Fraction det = 1;

    for (size_t i = 0; i < rows; i++) {
        // ищем строку с ненулевым элементом в столбце i
        size_t pivot = i;
        while (pivot < rows && temp.data[pivot][i] == 0) {
            pivot++;
        }
        if (pivot == rows) {
            return 0;
        }

        if (pivot != i) {
            std::swap(temp.data[i], temp.data[pivot]);
            det = -det;
        }
        det *= temp.data[i][i];

        for (size_t j = i + 1; j < rows; j++) {
            Fraction factor = temp.data[j][i] / temp.data[i][i];
            for (size_t k = i; k < rows; k++) {
                temp.data[j][k] -= temp.data[i][k] * factor;
            }
        }
    }
    return det;
}

//ранг
size_t Matrix::rank() {
    Matrix temp = *this;
    size_t rank = 0; // сколько опорных элементов нашли
    size_t row = 0;  // с какой строки ищем опорный

    for (size_t col = 0; col < columns && row < rows; col++) {
        // первая строка с ненулевым элементом в этом столбце
        size_t pivot = row;
        while (pivot < rows && temp.data[pivot][col] == 0) {
            pivot++;
        }
        if (pivot == rows) {
            continue; // весь столбец ниже нулевой
        }

        std::swap(temp.data[row], temp.data[pivot]);

        // обнуляем всё под опорным элементом
        for (size_t j = row + 1; j < rows; j++) {
            Fraction factor = temp.data[j][col] / temp.data[row][col];
            for (size_t k = col; k < columns; k++) {
                temp.data[j][k] -= temp.data[row][k] * factor;
            }
        }

        rank++;
        row++;
    }
    return rank;
}