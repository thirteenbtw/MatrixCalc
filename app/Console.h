#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include "Matrix.h"

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
inline int readInt(const char* text, int min, int max) {
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

inline double readDouble(const char* text) {
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

// ввод матрицы: сначала размеры, потом элементы по строкам
inline Matrix readMatrix(const char* name) {
    std::cout << "\nМатрица " << name << "\n";
    int r = readInt("Строк: ", 1, 20);
    int c = readInt("Столбцов: ", 1, 20);

    Matrix m(r, c);
    std::cout << "Вводите строки, числа через пробел (по " << c << " в строке)\n";
    for (int i = 0; i < r; i++) {
        while (true) {
            std::cout << "Строка " << i + 1 << ": ";
            bool ok = true;
            for (int j = 0; j < c; j++) {
                std::cin >> m.data[i][j];
                if (std::cin.fail()) {
                    ok = false;
                    break;
                }
            }
            if (std::cin.eof()) return m; // ввод закончился
            clearInput();
            if (ok) break;
            std::cout << "Нужно " << c << " чисел. Повторите строку\n";
        }
    }
    return m;
}
