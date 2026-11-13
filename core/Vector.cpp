#include "Vector.h"
#include "MathError.h"
#include <sstream>
#include <cmath>

Vector::Vector(size_t n) {
    if (n == 0) {
        throw MathError("Размер вектора должен быть больше нуля");
    }
    size = n;
    data = std::vector<Fraction>(n, 0);
}

std::string Vector::toString() {
    std::ostringstream out;
    out << "(";
    for (size_t i = 0; i < size; i++) {
        out << data[i];
        if (i + 1 < size) {
            out << ", ";
        }
    }
    out << ")";
    return out.str();
}

Vector Vector::add(Vector other) {
    if (size != other.size) {
        throw MathError("Нельзя сложить: разные размеры");
    }
    Vector result(size);
    for (size_t i = 0; i < size; i++) {
        result.data[i] = data[i] + other.data[i];
    }
    return result;
}

Vector Vector::subtract(Vector other) {
    if (size != other.size) {
        throw MathError("Нельзя вычесть: разные размеры");
    }
    Vector result(size);
    for (size_t i = 0; i < size; i++) {
        result.data[i] = data[i] - other.data[i];
    }
    return result;
}

Vector Vector::multiplyByNumber(Fraction number) {
    Vector result(size);
    for (size_t i = 0; i < size; i++) {
        result.data[i] = data[i] * number;
    }
    return result;
}

double Vector::length() {
    Fraction sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += data[i] * data[i];
    }
    return std::sqrt(sum.toDouble());
}

Fraction Vector::dot(Vector other) {
    if (size != other.size) {
        throw MathError("Скалярное произведение: разные размеры");
    }
    Fraction sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += data[i] * other.data[i];
    }
    return sum;
}

Vector Vector::cross(Vector other) {
    if (size != 3 or other.size != 3) {
        throw MathError("Векторное произведение есть только у векторов из 3 координат");
    }
    Vector result(3);
    result.data[0] = data[1] * other.data[2] - data[2] * other.data[1];
    result.data[1] = data[2] * other.data[0] - data[0] * other.data[2];
    result.data[2] = data[0] * other.data[1] - data[1] * other.data[0];
    return result;
}

double Vector::angle(Vector other) {
    double lengths = length() * other.length();
    if (lengths < 1e-9) {
        throw MathError("Угол с нулевым вектором не определён");
    }
    double c = dot(other).toDouble() / lengths;
    // из-за округления c может чуть выйти за [-1; 1], acos тогда сломается
    if (c > 1) c = 1;
    if (c < -1) c = -1;
    return std::acos(c) * 180.0 / 3.14159265358979;
}

Fraction Vector::mixed(Vector b, Vector c) {
    if (size != 3 or b.size != 3 or c.size != 3) {
        throw MathError("Смешанное произведение есть только у векторов из 3 координат");
    }
    return dot(b.cross(c));
}