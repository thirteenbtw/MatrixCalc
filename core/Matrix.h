#pragma once
#include <vector>
#include <string>
#include "Fraction.h"

class Matrix {
public:
    size_t rows;
    size_t columns;
    std::vector<std::vector<Fraction>> data; //data[строка][столбец]

    //конструктор создает матрицу r x c заполненную нулями
    Matrix(size_t r, size_t c);

    std::string toString(); //привидение к строке для вывода

    Matrix add(Matrix other); //сложение
    Matrix subtract(Matrix other); //вычитание
    Matrix multiplyByNumber(Fraction number); //умножение на число
    Matrix multiply(Matrix other);//умножение на матрицу
    Matrix transpose();//транспонирование
    Fraction determinant();//определитель
    size_t rank();//ранг
};