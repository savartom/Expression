#include <iostream>
#include <iomanip>

#include "../include/Expression.hpp"


void check(bool result)
{
    if(result)
    {
        std::cout << "OK\n";
    }
    else
    {
        std::cout << "FAIL\n";
    }
}


void testNumberConstructor()
{
    std::cout << std::left << std::setw(40) <<  "Number Constructor: ";
    Math::Expression<double> num1(1.123);
    Math::Expression<double> num2(2441);
    Math::Expression<double> num3(7649.12);

    using namespace std::complex_literals;
    Math::Expression<std::complex<double>> num4(10i);
    Math::Expression<std::complex<double>> num5(5.0 + 3i);
    Math::Expression<std::complex<double>> num6(543.12 + 659.32i);

    bool result = (num1.toString() == "1.123") && (num2.toString() == "2441")
        && (num3.toString() == "7649.12") && (num4.toString() == "10i")
        && (num5.toString() == "5 + 3i") && (num6.toString() == "543.12 + 659.32i");

    check(result);
}


void testVariableConstructor()
{
    std::cout << std::left << std::setw(40) <<  "Variable Constructor: ";
    Math::Expression<double> var1("x0");
    Math::Expression<double> var2("qwerty");
    Math::Expression<double> var3("ASD");

    Math::Expression<std::complex<double>> var4("variable");
    Math::Expression<std::complex<double>> var5("TheBestNameForVariable");
    Math::Expression<std::complex<double>> var6("AAAAAAAAAAAAAAAA");

    bool result = (var1.toString() == "x0") && (var2.toString() == "qwerty")
        && (var3.toString() == "ASD") && (var4.toString() == "variable")
        && (var5.toString() == "TheBestNameForVariable") && (var6.toString() == "AAAAAAAAAAAAAAAA");

    check(result);
}


void testStringConstructor()
{
    std::cout << std::left << std::setw(40) <<  "String Constructor: ";

    using namespace Math;
    using namespace std::complex_literals;

    bool result = (Expression<double>("cos(x) + sin(y)").toString() == "cos(x) + sin(y)")
        && (Expression<double>("sin(cos(ln(exp(z + x * y - r / w))))").toString() == "sin(cos(ln(exp(z + x * y - r / w))))")
        && (Expression<double>("2x^2 + y^(-1)").toString() == "2 * x^2 + y^(-1)")
        && (Expression<double>("(1 + 2) * 4 - 5 / 2").toString() == "(1 + 2) * 4 - 5 / 2")

        && (Expression<std::complex<double>>("i * a").toString() == "i * a")
        && (Expression<std::complex<double>>("x + 5y - sin(ln(z))").toString() == "x + 5 * y - sin(ln(z))")
        && (Expression<std::complex<double>>("(q^sin(5q)) / (y^(12))").toString() == "q^sin(5 * q) / y^12")
        && (Expression<std::complex<double>>("(cos(x) - sin(x)) * ln(x) / ln(x)").toString() == "(cos(x) - sin(x)) * ln(x) / ln(x)")
        && (Expression<std::complex<double>>("10 + i + x - y + w").toString() == "10 + i + x - y + w")
        && (Expression<std::complex<double>>("2cos(x) - 5sin(y) + 2(x) - 10exp(2y)").toString() == "2 * cos(x) - 5 * sin(y) + 2 * x - 10 * exp(2 * y)");

    check(result);
}


void testNumberNode()
{
    std::cout << std::left << std::setw(40) <<  "Number Node: ";

    using namespace std::complex_literals;
    using exd = Math::Expression<double>;
    using exc = Math::Expression<std::complex<double>>;

    exd num1(1.123);
    exd num2(2441);
    exc num4(10i);
    exc num5(5.0 + 3i);
    bool result = (num1.substitute("x", exd(123)).toString() == "1.123")
        && (num2.substitute("qwe", exd(654)).toString() == "2441")
        && (num4.substitute("zxc", exc(10i)).toString() == "10i")
        && (num5.substitute("Variable", exc(0.0 + 0i)).toString() == "5 + 3i")
        && (num1.calculate({}, {}) == 1.123)
        && (num2.calculate({"x"}, {123}) == 2441)
        && (num4.calculate({"asdfgh"}, {1000i}) == 10i);

    check(result);
}


void testVariableNode()
{
    std::cout << std::left << std::setw(40) <<  "Variable Node: ";

    using namespace std::complex_literals;
    using exd = Math::Expression<double>;
    using exc = Math::Expression<std::complex<double>>;

    Math::Expression<double> var1("x0");
    Math::Expression<double> var2("qwerty");
    Math::Expression<double> var3("ASD");

    Math::Expression<std::complex<double>> var4("variable");
    Math::Expression<std::complex<double>> var5("TheBestNameForVariable");
    Math::Expression<std::complex<double>> var6("AAAAAAAAAAAAAAAA");

    bool result = (var1.substitute("x0", exd(123)).toString() == "123")
        && (var2.substitute("x", exd(241)).toString() == "qwerty")
        && (var3.calculate({"ASD"}, {5}) == 5)
        && (var4.calculate({"variable"}, {0.0 + 0i}) == 0.0)
        && (var5.calculate({"TheBestNameForVariable"}, {2025i}) == 2025i)
        && (var6.substitute("AAAAAAAAAAAAAAAA", exc(10.0 + 10i)).toString() == "10 + 10i");

    check(result);
}


void testUnaryMinusNode()
{
    std::cout << std::left << std::setw(40) <<  "Unary Minus Node: ";
    Math::Expression<double> var1(2025.03);
    Math::Expression<double> var2("y0");

    using namespace std::complex_literals;
    Math::Expression<std::complex<double>> var3(-123.32 + 54i);
    Math::Expression<std::complex<double>> var4("temp");

    bool result = ((-var1).toString() == "-2025.03")
        && ((-var1).calculate({}, {}) == -2025.03)
        && ((-var2).toString() == "-y0")
        && ((-var2).calculate({"y0"}, {678}) == -678)
        && ((-var3).toString() == "-(-123.32 + 54i)")
        && ((-var3).calculate({}, {}) == 123.32 - 54i)
        && ((-var4).toString() == "-temp")
        && ((-var4).calculate({"temp"}, {100i}) == -100i);

    check(result);
}


void testAdditionSubtractionNode()
{
    std::cout << std::left << std::setw(40) <<  "Addition and Subtraction Nodes: ";

    using namespace std::complex_literals;
    using exd = Math::Expression<double>;
    using exc = Math::Expression<std::complex<double>>;

    exd var1 = -exd(123) + exd(23) - (exd("x") - exd("y"));
    exc var2 = exc(-1i) - exc(1i) + -exc("w") - exc("r");

    bool result = (var1.toString() == "-123 + 23 - (x - y)")
        && (var1.substitute("y", exd(-25)).toString() == "-125 - x")
        && (var1.calculate({"x", "y"}, {10, 10}) == -100)
        && (var2.toString() == "-i - i + (-w) - r")
        && (var2.substitute("w", exc(-1i)).toString() == "-i - r")
        && (var2.calculate({"r", "w"}, {10.0 + 0i, 5.0 - 3i}) == -15.0 + 1i);

    check(result);
}


void testMultiplicationDivisionNode()
{
    std::cout << std::left << std::setw(40) <<  "Multiplication and Division Nodes: ";

    using namespace Math;
    using namespace std::complex_literals;
    using exd = Expression<double>;
    using exc = Expression<std::complex<double>>;
    Expression<double> var1 = (exd(10) + exd(90)) * exd(0.01) - exd("x") / exd("y") * exd("z");
    Expression<std::complex<double>> var2 = exc(1i) * exc(-10i) / exc("x") + exc("z") * exc("y") / exc("x");

    bool result = (var1.toString() == "(10 + 90) * 0.01 - x / y * z")
        && (var1.substitute("y", exd("2")).toString() == "1 - x * z / 2")
        && (var1.calculate({"x", "y", "z"}, {1, 2, 3}) == -0.5)
        && (var2.toString() == "i * (-10i) / x + z * y / x")
        && (var2.substitute("x", exc("10.0 + 0i")).toString() == "1 + z * y / 10")
        && (var2.calculate({"x", "y", "z"}, {10.0 + 0i, 2i, 5i}) == 0.0 + 0i);

    check(result);
}


void testPowerNode()
{
    std::cout << std::left << std::setw(40) <<  "Power Node: ";

    using namespace Math;
    using namespace std::complex_literals;
    using exd = Expression<double>;
    using exc = Expression<std::complex<double>>;
    Expression<double> var1 = (exd(2)^exd("x")) + pow(exd(10) * exd("x"), exd(1) - exd("y"));
    Expression<std::complex<double>> var2 = pow(exc(1i), exc(2.0 + 0i)) + (exc("x")^exc("x"))*(exc("y")^exc(1i));

    bool result = (var1.toString() == "2^x + (10 * x)^(1 - y)")
        && (var1.substitute("x", exd(0.1)).toString() == "2^0.1 + 1")
        && (var1.calculate({"x", "y"}, {2, 2}) == 4.05)
        && (var2.toString() == "i^2 + x^x * y^i")
        && (var2.substitute("x", exc(1i)).toString() == "i^2 + i^i * y^i")
        && (std::abs(var2.calculate({"x", "y"}, {1i, 2.0 + 0i}) - (std::pow(1i, 2) + std::pow(1i, 1i) * std::pow(2, 1i))) < 0.001);

    check(result);
}


void testFunctionNode()
{
    std::cout << std::left << std::setw(40) <<  "Function Nodes: ";

    using namespace Math;
    using namespace std::complex_literals;
    using exd = Expression<double>;
    using exc = Expression<std::complex<double>>;
    exd var1 = sin(exd(1) + exd(0)) - cos(exd("x") - ln(exd("y"))) + exp(ln(exd(1)));
    exc var2 = sin(cos(exp(ln(exc("x") + exc(1.0 + 1i) - exc("y")))));

    bool result = (var1.toString() == "sin(1 + 0) - cos(x - ln(y)) + exp(ln(1))")
        && (var1.substitute("y", exd(3)).toString() == "sin(1) - cos(x - ln(3)) + 1")
        && (var1.calculate({"x", "y"}, {2, 1}) == std::sin(1) - std::cos(2) + 1)
        && (var2.toString() == "sin(cos(exp(ln(x + 1 + i - y))))")
        && (var2.substitute("y", exc(1.0 + 1i)).toString() == "sin(cos(x))")
        && (std::abs(var2.calculate({"x", "y"}, {10.0 + 0i, 5i}) - std::sin(std::cos(std::exp(std::log(11.0 - 4i))))) < 0.02);

    check(result);
}


void testSubstitute()
{
    std::cout << std::left << std::setw(40) <<  "Substitute: ";

    using namespace Math;
    using namespace std::complex_literals;
    using exd = Expression<double>;
    using exc = Expression<std::complex<double>>;
    exd x("x"), y("Y"), z("zzz");
    exd var1 = sin(x + cos(x * ln(x / exp(x - y + (-z)))))^pow(z, y + z * cos(exp(ln(-x - z - y))));
    exc w("w"), v("variable"), r("r2025");
    exc var2 = exp(v + v - sin(w) * cos(r)) / ln(pow(r, w) * cos(w * v * r) - sin(sin(w)));

    bool result = (var1.toString() == "sin(x + cos(x * ln(x / exp(x - Y + (-zzz)))))^(zzz^(Y + zzz * cos(exp(ln(-x - zzz - Y)))))")
        && (var1.substitute("x", exd(10)).toString() == "sin(10 + cos(10 * (ln(10) - (10 - Y - zzz))))^(zzz^(Y + zzz * cos(-10 - zzz - Y)))")
        && (var1.substitute("Y", exd(-1)).toString() == "sin(x + cos(x * (ln(x) - (x - (-1) - zzz))))^(zzz^(-1 + zzz * cos(x + zzz + (-1))))")
        && (var1.substitute("zzz", exd(-5.1)).toString() == "sin(x + cos(x * (ln(x) - (x - Y + 5.1))))^((-5.1)^(Y + (-5.1 * cos(x + (-5.1) + Y))))")
        && (var2.toString() == "exp(variable + variable - sin(w) * cos(r2025)) / ln(r2025^w * cos(w * variable * r2025) - sin(sin(w)))")
        && (var2.substitute("w", exc(-1.0 - 1i)).toString() == "exp(2 * variable - sin(-1 - i) * cos(r2025)) / ln(r2025^(-1 - i) * cos((-1 - i) * variable * r2025) - sin(sin(-1 - i)))")
        && (var2.substitute("variable", exc(1.23i)).toString() == "exp(2.46i - sin(w) * cos(r2025)) / ln(r2025^w * cos(w * 1.23i * r2025) - sin(sin(w)))")
        && (var2.substitute("r2025", exc(-3.0 - 0i)).toString() == "exp(2 * variable - sin(w) * cos(-3)) / ln((-3)^w * cos(w * variable * (-3)) - sin(sin(w)))");

    check(result);
}


void testCalculate()
{
    std::cout << std::left << std::setw(40) <<  "Calculate: ";

    using namespace Math;
    using namespace std::complex_literals;
    using exd = Expression<double>;
    using exc = Expression<std::complex<double>>;
    exd x("x"), y("y"), z("z");
    exd var1 = cos(sin(x)) - ln(exp(x)) + pow(x, y) - cos(exp(sin(x * y * z)));
    exc w("w"), v("v"), r("r");
    exc var2 = pow(sin(w + r), ln(-cos(r / w))) * (w + r - v) + cos(r / v) / ln(w) - w / w + ln(exp(w));

    bool result = true;

    for(int x2 = 0; x2 < 10; x2++)
    {
        for(int y2 = 0; y2 < 10; y2++)
        {
            for(int z2 = 0; z2 < 10; z2++)
            {
                double x1 = x2 + 0.123, y1 = y2 - 3.141, z1 = z2;

                result = result && (var1.calculate({"x", "y", "z"}, {x1, y1, z1}) ==
                    std::cos(std::sin(x1)) - std::log(std::exp(x1)) + std::pow(x1, y1) - std::cos(std::exp(std::sin(x1 * y1 * z1))));
                std::complex<double> w1 = x1 * y1 - z1 * 1i;
                std::complex<double> v1 = y1 * z1 + x1 * 1i;
                std::complex<double> r1 = z1 - x1 * y1 * 1i;
                result = result && (var2.calculate({"w", "v", "r"}, {w1, v1, r1}) ==
                    std::pow(std::sin(w1 + r1), std::log(-std::cos(r1 / w1))) * (w1 + r1 - v1) +
                    std::cos(r1 / v1) / std::log(w1) - w1 / w1 + std::log(std::exp(w1)));
            }
        }
    }

    check(result);
}


void testSimplify()
{
    std::cout << std::left << std::setw(40) <<  "Simplify: ";
    using namespace std::complex_literals;
    using ex = Math::Expression<std::complex<double>>;
    ex x("x"), y("y"), z("z"), a("a"), b("b"), c("c");
    ex zero(0i), one(1.0 + 0i), two(2.0 + 0i), three(3.0 + 0i), four(4.0 + 0i);

    bool result = ((zero * one + two - three / four).simplify().toString() == "1.25")
        && ((zero + x).simplify().toString() == "x")
        && ((x + zero).simplify().toString() == "x")
        && ((-x + (-y)).simplify().toString() == "-(x + y)")
        && ((-x + y).simplify().toString() == "y - x")
        && ((x + (-y)).simplify().toString() == "x - y")
        && ((x + x).simplify().toString() == "2 * x")
        && (((sin(x)^two) + (cos(x)^two)).simplify().toString() == "1")
        && ((zero - x = -x).simplify().toString() == "-x")
        && ((x - zero).simplify().toString() == "x")
        && ((x - x).simplify().toString() == "0")
        && ((-x - y).simplify().toString() == "-(x + y)")
        && ((x - (-y)).simplify().toString() == "x + y")
        && (((x * a) + (x * b)).simplify().toString() == "(a + b) * x")
        && (((x * a) + (b * x)).simplify().toString() == "(a + b) * x")
        && (((a * x) + (x * b)).simplify().toString() == "(a + b) * x")
        && (((a * x) + (b * x)).simplify().toString() == "(a + b) * x")
        && (((a * x) + x).simplify().toString() == "(a + 1) * x")
        && (((x * a) + x).simplify().toString() == "(a + 1) * x")
        && ((x + (a * x)).simplify().toString() == "(a + 1) * x")
        && ((x + (x * a)).simplify().toString() == "(a + 1) * x")
        && ((a / x + b / x).simplify().toString() == "(a + b) / x")
        && (((x * a) - (x * b)).simplify().toString() == "(a - b) * x")
        && (((x * a) - (b * x)).simplify().toString() == "(a - b) * x")
        && (((a * x) - (x * b)).simplify().toString() == "(a - b) * x")
        && (((a * x) - (b * x)).simplify().toString() == "(a - b) * x")
        && (((x * a) - x).simplify().toString() == "(a - 1) * x")
        && (((a * x) - x).simplify().toString() == "(a - 1) * x")
        && ((x - (x * a)).simplify().toString() == "(1 - a) * x")
        && ((x - (a * x)).simplify().toString() == "(1 - a) * x")
        && ((a / x - b / x).simplify().toString() == "(a - b) / x")
        && ((zero * x).simplify().toString() == "0")
        && ((x * zero).simplify().toString() == "0")
        && ((one * x).simplify().toString() == "x")
        && ((x * one).simplify().toString() == "x")
        && ((-one * x).simplify().toString() == "-x")
        && ((x * -one).simplify().toString() == "-x")
        && ((x * x).simplify().toString() == "x^2")
        && ((-x * -y).simplify().toString() == "x * y")
        && (((a / x) * (b / y)).simplify().toString() == "a * b / (x * y)")
        && ((x * (a / x)).simplify().toString() == "a")
        && (((x^b) * (a / x)).simplify().toString() == "a * x^(b - 1)")
        && (((x^b) * (a / (x^c))).simplify().toString() == "a * x^(b - c)")
        && (((a * x) * x).simplify().toString() == "a * x^2")
        && (((x * a) * x).simplify().toString() == "a * x^2")
        && ((x * (x * a)).simplify().toString() == "a * x^2")
        && ((x * (a * x)).simplify().toString() == "a * x^2")
        && (((a * (x^b)) * x).simplify().toString() == "a * x^(b + 1)")
        && ((x * (a * (x^b))).simplify().toString() == "a * x^(b + 1)")
        && (((a * (x^b)) * (x^c)).simplify().toString() == "a * x^(b + c)")
        && ((((x^b) * a) * x).simplify().toString() == "a * x^(b + 1)")
        && ((((x^b) * a) * (x^c)).simplify().toString() == "a * x^(b + c)")
        && ((x * ((x^b) * a)).simplify().toString() == "a * x^(b + 1)")
        && (((x^c) * (a * (x^b))).simplify().toString() == "a * x^(b + c)")
        && (((x^c) * ((x^b) * a)).simplify().toString() == "a * x^(b + c)")
        && (((x^a) * x).simplify().toString() == "x^(a + 1)")
        && ((x * (x^a)).simplify().toString() == "x^(a + 1)")
        && (((x^a) * (x^b)).simplify().toString() == "x^(a + b)")
        && ((zero / x).simplify().toString() == "0")
        && ((x / one).simplify().toString() == "x")
        && ((x / -one).simplify().toString() == "-x")
        && ((x / x).simplify().toString() == "1")
        && ((-x / -y).simplify().toString() == "x / y")
        && (((x / a) / (y / b)).simplify().toString() == "x * b / (a * y)")
        && (((a / b) / x).simplify().toString() == "a / (b * x)")
        && ((x / (a / b)).simplify().toString() == "x * b / a")
        && (((x * a) / (b * x)).simplify().toString() == "a / b")
        && (((x * a) / (x * b)).simplify().toString() == "a / b")
        && (((a * x) / (b * x)).simplify().toString() == "a / b")
        && (((a * x) / (x * b)).simplify().toString() == "a / b")
        && (((x * a) / x).simplify().toString() == "a")
        && (((a * x) / x).simplify().toString() == "a")
        && (((a * (x^b)) / x).simplify().toString() == "a * x^(b - 1)")
        && (((a * (x^b)) / (x^c)).simplify().toString() == "a * x^(b - c)")
        && ((((x^b) * a) / x).simplify().toString() == "a * x^(b - 1)")
        && ((((x^b) * a) / (x^c)).simplify().toString() == "a * x^(b - c)")
        && ((x / (x * a)).simplify().toString() == "1 / a")
        && ((x / (a * x)).simplify().toString() == "1 / a")
        && ((x / (a * (x^b))).simplify().toString() == "x^(1 - b) / a")
        && (((x^c) / (a * (x^b))).simplify().toString() == "x^(c - b) / a")
        && ((x / ((x^b) * a)).simplify().toString() == "x^(1 - b) / a")
        && (((x^c) / ((x^b) * a)).simplify().toString() == "x^(c - b) / a")
        && (((x^a) / (x^b)).simplify().toString() == "x^(a - b)")
        && (((x^a) / x).simplify().toString() == "x^(a - 1)")
        && ((x / (x^a)).simplify().toString() == "x^(1 - a)")
        && ((zero^x).simplify().toString() == "0")
        && ((one^x).simplify().toString() == "1")
        && ((x^one).simplify().toString() == "x")
        && ((x^zero).simplify().toString() == "1")
        && (((x^a)^b).simplify().toString() == "x^(a * b)")
        && ((-(-x)).simplify().toString() == "x")
        && ((cos(-x)).simplify().toString() == "cos(x)")
        && ((sin(-x)).simplify().toString() == "-sin(x)")
        && ((exp(ln(x))).simplify().toString() == "x")
        && ((exp(a * ln(x))).simplify().toString() == "x^a")
        && ((exp(ln(x) * a)).simplify().toString() == "x^a")
        && ((exp(ln(x) / a)).simplify().toString() == "x^(1 / a)")
        && ((exp(ln(x) + y)).simplify().toString() == "x * exp(y)")
        && ((exp(y + ln(x))).simplify().toString() == "x * exp(y)")
        && ((exp(ln(x) - y)).simplify().toString() == "x / exp(y)")
        && ((exp(y - ln(x))).simplify().toString() == "exp(y) / x")
        && ((ln(exp(x))).simplify().toString() == "x")
        && ((ln(exp(x) * a)).simplify().toString() == "x + ln(a)")
        && ((ln(a * exp(x))).simplify().toString() == "x + ln(a)")
        && ((ln(exp(x) / a)).simplify().toString() == "x - ln(a)")
        && ((ln(a / exp(x))).simplify().toString() == "ln(a) - x")
        && ((a * (b / c)).simplify().toString() == "a * b / c")
        && (((a / b) * c).simplify().toString() == "a * c / b")
        && ((x).simplify().toString() == "x");

    check(result);
}


void testDifferentiate()
{
    std::cout << std::left << std::setw(40) <<  "Differentiate: ";

    using namespace std::complex_literals;
    using ex = Math::Expression<std::complex<double>>;
    ex x("x"), y("y"), z("z"), a("a"), b("b"), c("c");
    ex zero(0i), one(1.0 + 0i), two(2.0 + 0i), three(3.0 + 0i);

    bool result = ((x).differentiate().toString() == "1")
        && ((cos(y / z) * sin(ln(z^z)) + exp(two - y * z)).differentiate().toString() == "0")
        && ((x + a + b + c).differentiate().toString() == "1")
        && ((x^two).differentiate().toString() == "2 * x")
        && ((x * x * x).differentiate().toString() == "3 * x^2")
        && ((a * (x^two) + b * x + c).differentiate().toString() == "a * 2 * x + b")

        && (((a * x + b)^two).differentiate().toString() == "2 * a * (a * x + b)")
        && ((a * x + b).differentiate().toString() == "a")
        && (((a * x + b)^(-one)).differentiate().toString() == "-a * (a * x + b)^(-2)")
        && (((a * x + b)^(-two)).differentiate().toString() == "-2 * a * (a * x + b)^(-3)")

        && ((sin(x)).differentiate().toString() == "cos(x)")
        && ((cos(x)).differentiate().toString() == "-sin(x)")
        && ((sin(x) / cos(x)).differentiate().toString() == "1 / cos(x)^2")
        && ((cos(x) / sin(x)).differentiate().toString() == "-1 / sin(x)^2")
        && ((x * sin(x)).differentiate().toString() == "sin(x) + x * cos(x)")
        && (((x^two) * cos(x)).differentiate().toString() == "2 * x * cos(x) - x^2 * sin(x)")
        && (((sin(x) * cos(x))).differentiate().toString() == "cos(x)^2 - sin(x)^2")
        && ((cos(x) - sin(x)).differentiate().toString() == "-(sin(x) + cos(x))")

        && (ln(x).differentiate().toString() == "1 / x")
        && (exp(x).differentiate().toString() == "exp(x)")
        && ((x^x).differentiate().toString() == "(1 + ln(x)) * x^x")
        && ((x^a).differentiate().toString() == "a * x^(a - 1)")
        && ((a^x).differentiate().toString() == "ln(a) * a^x")
        && ((sin(x) * ln(x)).differentiate().toString() == "cos(x) * ln(x) + sin(x) / x")
        && ((cos(x) / exp(x)).differentiate().toString() == "-(sin(x) + cos(x)) * exp(x) / exp(x)^2")
        && (sin(cos(x)).differentiate().toString() == "-cos(cos(x)) * sin(x)")
        && ((ln(x)^cos(x)).differentiate().toString() == "(cos(x) / (x * ln(x)) - sin(x) * ln(ln(x))) * ln(x)^cos(x)");

    check(result);
}


int main()
{
    testNumberConstructor();
    testVariableConstructor();
    testStringConstructor();
    testNumberNode();
    testVariableNode();
    testUnaryMinusNode();
    testAdditionSubtractionNode();
    testMultiplicationDivisionNode();
    testPowerNode();
    testFunctionNode();
    testSubstitute();
    testCalculate();
    testSimplify();
    testDifferentiate();
}
