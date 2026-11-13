#include "Fraction.h"
#include "MathError.h"
#include <numeric>
#include <stdexcept>

// конструктор сокращает дробь и следит за знаком
Fraction::Fraction(long long n, long long d) {
    if (d == 0) {
        throw MathError("Знаменатель не может быть равен нулю");
    }
    if (d < 0) {
        n = -n;
        d = -d;
    }
    long long g = std::gcd(n, d); // для n = 0 получится d, дробь станет 0/1
    num = n / g;
    den = d / g;
}

double Fraction::toDouble() const {
    return (double)num / den;
}

std::string Fraction::toString() const {
    if (den == 1) {
        return std::to_string(num);
    }
    return std::to_string(num) + "/" + std::to_string(den);
}

Fraction Fraction::parse(const std::string& s) {
    try {
        size_t pos;

        // вид "a/b"
        size_t slash = s.find('/');
        if (slash != std::string::npos) {
            std::string a = s.substr(0, slash);
            std::string b = s.substr(slash + 1);
            size_t posA, posB;
            long long n = std::stoll(a, &posA);
            long long d = std::stoll(b, &posB);
            if (posA != a.size() or posB != b.size()) {
                throw MathError("Неверная дробь");
            }
            return Fraction(n, d);
        }

        // обычное целое
        size_t dot = s.find_first_of(".,");
        if (dot == std::string::npos) {
            long long n = std::stoll(s, &pos);
            if (pos != s.size()) {
                throw MathError("Неверное число");
            }
            return Fraction(n);
        }

        // десятичное: "-2.5" -> -25 / 10
        std::string whole = s.substr(0, dot);
        std::string frac = s.substr(dot + 1);
        if (frac.size() > 15) {
            throw MathError("Слишком много знаков после запятой");
        }
        long long n = std::stoll(whole + frac, &pos);
        if (pos != whole.size() + frac.size()) {
            throw MathError("Неверное число");
        }
        long long d = 1;
        for (size_t i = 0; i < frac.size(); i++) {
            d *= 10;
        }
        return Fraction(n, d);
    }
    catch (std::invalid_argument&) {
        throw MathError("Неверное число");
    }
    catch (std::out_of_range&) {
        throw MathError("Слишком большое число");
    }
}

Fraction Fraction::operator-() const {
    return Fraction(-num, den);
}

Fraction& Fraction::operator+=(const Fraction& other) {
    *this = Fraction(num * other.den + other.num * den, den * other.den);
    return *this;
}

Fraction& Fraction::operator-=(const Fraction& other) {
    *this = Fraction(num * other.den - other.num * den, den * other.den);
    return *this;
}

Fraction& Fraction::operator*=(const Fraction& other) {
    *this = Fraction(num * other.num, den * other.den);
    return *this;
}

Fraction& Fraction::operator/=(const Fraction& other) {
    if (other.num == 0) {
        throw MathError("Деление на ноль");
    }
    *this = Fraction(num * other.den, den * other.num);
    return *this;
}

Fraction operator+(Fraction a, const Fraction& b) { a += b; return a; }
Fraction operator-(Fraction a, const Fraction& b) { a -= b; return a; }
Fraction operator*(Fraction a, const Fraction& b) { a *= b; return a; }
Fraction operator/(Fraction a, const Fraction& b) { a /= b; return a; }

// дроби всегда сокращены, поэтому равенство - это совпадение обеих частей
bool operator==(const Fraction& a, const Fraction& b) {
    return a.num == b.num and a.den == b.den;
}
bool operator!=(const Fraction& a, const Fraction& b) {
    return !(a == b);
}
// знаменатели положительные, поэтому можно сравнивать "крест-накрест"
bool operator<(const Fraction& a, const Fraction& b) {
    return a.num * b.den < b.num * a.den;
}
bool operator>(const Fraction& a, const Fraction& b) {
    return b < a;
}

Fraction abs(const Fraction& f) {
    return Fraction(f.num < 0 ? -f.num : f.num, f.den);
}

std::ostream& operator<<(std::ostream& out, const Fraction& f) {
    return out << f.toString();
}