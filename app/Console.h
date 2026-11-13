#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include "Matrix.h"
#include "Vector.h"
#include "MathError.h"

inline void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

// если пользователь ввёл не число - сбрасываем ошибку и выкидываем остаток строки
inline void clearInput() {
    std::cin.clear();
    std::cin.ignore(1000, '\n');
}

// спрашивает целое число от min до max, пока не введут правильно
inline int readInt(const char *text, int min, int max) {
    int x;
    while (true) {
        std::cout << text;
        std::cin >> x;
        if (std::cin.fail() || x < min || x > max) {
            std::cout << "Введите целое число от " << min << " до " << max << "\n";
            clearInput();
        } else {
            clearInput();
            return x;
        }
    }
}

inline double readDouble(const char *text) {
    double x;
    while (true) {
        std::cout << text;
        std::cin >> x;
        if (std::cin.fail()) {
            std::cout << "Введите число\n";
            clearInput();
        } else {
            clearInput();
            return x;
        }
    }
}

// читает count чисел/дробей в row; false - если что-то не число
inline bool readRow(std::vector<Fraction>& row, int count) {
    for (int j = 0; j < count; j++) {
        std::string token;
        std::cin >> token;
        if (std::cin.fail()) return false;
        try {
            row[j] = Fraction::parse(token);
        }
        catch (MathError&) {
            return false;
        }
    }
    return true;
}

// ввод матрицы: сначала размеры, потом элементы по строкам
// needRows / needCols: обязательные размеры (0 - любые)
// mustBeSquare: матрица обязана быть квадратной
inline Matrix readMatrix(const char *name, int needRows = 0, int needCols = 0, bool mustBeSquare = false) {
    std::cout << "\nМатрица " << name << "\n";
    int r = readInt("Строк: ", 1, 20);
    if (needRows != 0 and r != needRows) {
        throw MathError("Число строк должно быть " + std::to_string(needRows));
    }

    int c = readInt("Столбцов: ", 1, 20);
    if (needCols != 0 and c != needCols) {
        throw MathError("Число столбцов должно быть " + std::to_string(needCols));
    }
    if (mustBeSquare and r != c) {
        throw MathError("Матрица должна быть квадратной");
    }

    Matrix m(r, c);
    std::cout << "Вводите строки, числа через пробел (по " << c << " в строке)\n";
    for (int i = 0; i < r; i++) {
        while (true) {
            std::cout << "Строка " << i + 1 << ": ";
            bool ok = readRow(m.data[i], c);
            if (std::cin.eof()) return m; // ввод закончился
            clearInput();
            if (ok) break;
            std::cout << "Нужно " << c << " чисел. Повторите строку\n";
        }
    }
    return m;
}

// ввод вектора: размерность, потом координаты через пробел
// needSize: какая размерность обязательна (0 - любая)
inline Vector readVector(const char *name, int needSize = 0) {
    std::cout << "\nВектор " << name << "\n";
    int n = readInt("Размерность: ", 1, 20);

    if (needSize != 0 and n != needSize) {
        throw MathError("Размерность должна быть " + std::to_string(needSize));
    }

    Vector v(n);
    std::cout << "Введите " << n << " чисел через пробел\n";
    while (true) {
        std::cout << name << ": ";
        bool ok = readRow(v.data, n);
        if (std::cin.eof()) return v; // ввод закончился
        clearInput();
        if (ok) break;
        std::cout << "Нужно " << n << " чисел. Повторите\n";
    }
    return v;
}

// спрашивает число или дробь: 3, -2.5, 1/3
inline Fraction readFraction(const char* text) {
    std::string s;
    while (true) {
        std::cout << text;
        std::cin >> s;
        if (std::cin.fail()) return 0; // ввод закончился
        clearInput();
        try {
            return Fraction::parse(s);
        }
        catch (MathError&) {
            std::cout << "Введите число или дробь, например 3, -2.5 или 1/3\n";
        }
    }
}
