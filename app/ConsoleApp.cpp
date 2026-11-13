#include <iostream>
#include "ConsoleApp.h"
#include "Matrix.h"
#include "Vector.h"
#include "Solver.h"
#include "MathError.h"
#include "Console.h"

void matrixMenu() {
    while (true) {
        std::cout << "\n===== Матрицы =====\n";
        std::cout << "1. Сложение A + B\n";
        std::cout << "2. Вычитание A - B\n";
        std::cout << "3. Умножение A * B\n";
        std::cout << "4. Умножение на число\n";
        std::cout << "5. Транспонирование\n";
        std::cout << "6. Определитель\n";
        std::cout << "7. Ранг\n";
        std::cout << "0. Назад\n";

        int choice = readInt("Выберите операцию: ", 0, 7);
        if (choice == 0) {
            break;
        }

        try {
            // определитель есть только у квадратной матрицы
            Matrix a = readMatrix("A", 0, 0, choice == 6);

            if (choice == 1) {
                Matrix b = readMatrix("B", a.rows, a.columns);
                std::cout << "\nРезультат:\n" << a.add(b).toString() << "\n";
            } else if (choice == 2) {
                Matrix b = readMatrix("B", a.rows, a.columns);
                std::cout << "\nРезультат:\n" << a.subtract(b).toString() << "\n";
            } else if (choice == 3) {
                // строк у B должно быть столько же, сколько столбцов у A
                Matrix b = readMatrix("B", a.columns);
                std::cout << "\nРезультат:\n" << a.multiply(b).toString() << "\n";
            } else if (choice == 4) {
                Fraction k = readFraction("Число: ");
                std::cout << "\nРезультат:\n" << a.multiplyByNumber(k).toString() << "\n";
            } else if (choice == 5) {
                std::cout << "\nРезультат:\n" << a.transpose().toString() << "\n";
            } else if (choice == 6) {
                std::cout << "\nОпределитель: " << a.determinant() << "\n";
            } else if (choice == 7) {
                std::cout << "\nРанг матрицы: " << a.rank() << "\n";
            }
        } catch (MathError &e) {
            std::cout << "\nОшибка: " << e.what() << "\n";
        } catch (std::exception &e) {
            std::cout << "\nОшибка: " << e.what() << "\n";
        }
    }
}

void vectorMenu() {
    while (true) {
        std::cout << "\n===== Векторы =====\n";
        std::cout << "1. Сложение a + b\n";
        std::cout << "2. Вычитание a - b\n";
        std::cout << "3. Умножение на число\n";
        std::cout << "4. Длина\n";
        std::cout << "5. Скалярное произведение\n";
        std::cout << "6. Векторное произведение\n";
        std::cout << "7. Угол между векторами\n";
        std::cout << "8. Смешанное произведение\n";
        std::cout << "0. Назад\n";

        int choice = readInt("Выберите операцию: ", 0, 8);
        if (choice == 0) {
            break;
        }

        try {
            // для векторного и смешанного произведения нужны 3 координаты
            int need = (choice == 6 or choice == 8) ? 3 : 0;
            Vector a = readVector("a", need);

            if (choice == 1) {
                Vector b = readVector("b", a.size);
                std::cout << "\nРезультат: " << a.add(b).toString() << "\n";
            } else if (choice == 2) {
                Vector b = readVector("b", a.size);
                std::cout << "\nРезультат: " << a.subtract(b).toString() << "\n";
            } else if (choice == 3) {
                Fraction k = readFraction("Число: ");
                std::cout << "\nРезультат: " << a.multiplyByNumber(k).toString() << "\n";
            } else if (choice == 4) {
                std::cout << "\nДлина: " << a.length() << "\n";
            } else if (choice == 5) {
                Vector b = readVector("b", a.size);
                std::cout << "\nСкалярное произведение: " << a.dot(b) << "\n";
            } else if (choice == 6) {
                Vector b = readVector("b", a.size);
                std::cout << "\nРезультат: " << a.cross(b).toString() << "\n";
            } else if (choice == 7) {
                Vector b = readVector("b", a.size);
                std::cout << "\nУгол: " << a.angle(b) << " градусов\n";
            } else if (choice == 8) {
                Vector b = readVector("b", 3);
                Vector c = readVector("c", 3);
                std::cout << "\nСмешанное произведение: " << a.mixed(b, c) << "\n";
            }
        } catch (MathError &e) {
            std::cout << "\nОшибка: " << e.what() << "\n";
        } catch (std::exception &e) {
            std::cout << "\nОшибка: " << e.what() << "\n";
        }
    }
}

void solveMenu() {
    std::cout << "\n===== Решение СЛАУ (метод Гаусса) =====\n";
    std::cout << "Система вида A * x = b\n";

    try {
        Matrix a = readMatrix("A");
        // размер b должен совпасть с числом уравнений (строк A)
        Vector b = readVector("b", a.rows);

        Solution s = solveGauss(a, b);

        if (s.type == SolutionType::None) {
            std::cout << "\nРешений нет\n";
        } else if (s.type == SolutionType::Unique) {
            std::cout << "\nЕдинственное решение: x = " << s.x.toString() << "\n";
        } else {
            std::cout << "\nБесконечно много решений.\n";
            std::cout << "Общее решение:\n";
            std::cout << "x = " << s.x.toString();
            for (size_t i = 0; i < s.basis.size(); i++) {
                std::cout << " + t" << i + 1 << " * " << s.basis[i].toString();
            }
            std::cout << "\n";

            std::cout << "где t1.. - любые числа, t соответствует свободным переменным:";
            for (size_t i = 0; i < s.freeCols.size(); i++) {
                std::cout << " t" << i + 1 << " = x" << s.freeCols[i] + 1;
            }
            std::cout << "\n";
        }
    } catch (MathError &e) {
        std::cout << "\nОшибка: " << e.what() << "\n";
    } catch (std::exception &e) {
        std::cout << "\nОшибка: " << e.what() << "\n";
    }
}

void runConsole() {
    setupConsole();

    while (true) {
        std::cout << "\n===== Калькулятор =====\n";
        std::cout << "1. Матрицы\n";
        std::cout << "2. Векторы\n";
        std::cout << "3. Решение СЛАУ\n";
        std::cout << "0. Выход\n";

        int mode = readInt("Выберите режим: ", 0, 3);
        if (mode == 0) {
            break;
        } else if (mode == 1) {
            matrixMenu();
        } else if (mode == 2) {
            vectorMenu();
        } else {
            solveMenu();
        }
    }
}
