#include <iostream>
#include "Matrix.h"
#include "MathError.h"
#include "Console.h"

int main() {
    setupConsole();

    while (true) {
        std::cout << "\n===== Матричный калькулятор =====\n";
        std::cout << "1. Сложение A + B\n";
        std::cout << "2. Вычитание A - B\n";
        std::cout << "3. Умножение A * B\n";
        std::cout << "4. Умножение на число\n";
        std::cout << "5. Транспонирование\n";
        std::cout << "6. Определитель\n";
        std::cout << "0. Выход\n";

        int choice = readInt("Выберите операцию: ", 0, 6);
        if (choice == 0) {
            break;
        }

        try {
            Matrix a = readMatrix("A");

            if (choice == 1) {
                Matrix b = readMatrix("B");
                std::cout << "\nРезультат:\n" << a.add(b).toString() << "\n";
            } else if (choice == 2) {
                Matrix b = readMatrix("B");
                std::cout << "\nРезультат:\n" << a.subtract(b).toString() << "\n";
            } else if (choice == 3) {
                Matrix b = readMatrix("B");
                std::cout << "\nРезультат:\n" << a.multiply(b).toString() << "\n";
            } else if (choice == 4) {
                double k = readDouble("Число: ");
                std::cout << "\nРезультат:\n" << a.multiplyByNumber(k).toString() << "\n";
            } else if (choice == 5) {
                std::cout << "\nРезультат:\n" << a.transpose().toString() << "\n";
            } else if (choice == 6) {
                std::cout << "\nОпределитель: " << a.determinant() << "\n";
            }
        }
        catch (MathError& e) {
            std::cout << "\nОшибка: " << e.what() << "\n";
        }
        catch (std::exception& e) {
            std::cout << "\nОшибка: " << e.what() << "\n";
        }
    }
}
