#pragma once
#include <vector>
#include <string>

class Matrix {
public:
    size_t rows;
    size_t columns;
    std::vector<std::vector<double>> data; //data[строка][столбец]

    //конструктор создает матрицу r x c заполненную нулями
    Matrix(size_t r, size_t c);

    std::string toString(); //привидение к строке для вывода

    Matrix add(Matrix other); //сложение
    Matrix subtract(Matrix other); //вычитание
    Matrix multiplyByNumber(double number); //умножение на скаляр
    Matrix multiply(Matrix other);//умножение на матрицу
    Matrix transpose();//транспонирование
    double determinant();//определитель
};