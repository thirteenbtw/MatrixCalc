#pragma once
#include <string>
#include <ostream>

// обыкновенная дробь num/den, всегда сокращена, знаменатель > 0
class Fraction {
public:
    long long num; // числитель (знак хранится здесь)
    long long den; // знаменатель

    Fraction(long long n = 0, long long d = 1);

    double toDouble() const;
    std::string toString() const;

    // "3", "-2.5", "1/3", "-7/2"
    static Fraction parse(const std::string& s);

    Fraction operator-() const;
    Fraction& operator+=(const Fraction& other);
    Fraction& operator-=(const Fraction& other);
    Fraction& operator*=(const Fraction& other);
    Fraction& operator/=(const Fraction& other);
};

Fraction operator+(Fraction a, const Fraction& b);
Fraction operator-(Fraction a, const Fraction& b);
Fraction operator*(Fraction a, const Fraction& b);
Fraction operator/(Fraction a, const Fraction& b);

bool operator==(const Fraction& a, const Fraction& b);
bool operator!=(const Fraction& a, const Fraction& b);
bool operator<(const Fraction& a, const Fraction& b);
bool operator>(const Fraction& a, const Fraction& b);

Fraction abs(const Fraction& f);
std::ostream& operator<<(std::ostream& out, const Fraction& f);