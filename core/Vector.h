#pragma once
#include <vector>
#include <string>
#include "Fraction.h"

class Vector {
public:
    size_t size;
    std::vector<Fraction> data;

    //конструктор создает вектор из n нулей
    Vector(size_t n);

    std::string toString(); //привидение к строке для вывода

    Vector add(Vector other); //сложение
    Vector subtract(Vector other); //вычитание
    Vector multiplyByNumber(Fraction number); //умножение на число
    double length(); //длина (корень - уже не дробь)
    Fraction dot(Vector other); //скалярное произведение
    Vector cross(Vector other); //векторное произведение (только для 3 координат)
    double angle(Vector other); //угол между векторами в градусах
    Fraction mixed(Vector b, Vector c); //смешанное произведение (a, b, c)
};