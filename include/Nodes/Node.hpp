#ifndef NODE_HPP
#define NODE_HPP


#include <memory>
#include <complex>
#include <vector>


namespace Math
{
    template<typename Type>
    Type getNumber(double first, double second=0)
    {
        return first;
    }


    template<>
    inline std::complex<float> getNumber<std::complex<float>>(double first, double second)
    {
        return {static_cast<float>(first), static_cast<float>(second)};
    }


    template<>
    inline std::complex<double> getNumber<std::complex<double>>(double first, double second)
    {
        return {first, second};
    }


    template<>
    inline std::complex<long double> getNumber<std::complex<long double>>(double first, double second)
    {
        return {first, second};
    }


    std::string to_string(auto x)
    {
        std::stringstream sx;
        std::string result;
        sx << x;
        sx >> result;
        return result;
    };


    enum class Priority
    {
        Addition,
        Subtraction, Minus=Subtraction,
        Multiplication, Division=Multiplication,
        Power,
        Variable, Sin=Variable, Cos=Variable, Exp=Variable, Ln=Variable, Number=Variable
    };


    enum class TypeNode
    {
        Variable, Number,
        Addition, Subtraction, Minus,
        Multiplication, Division,
        Power, Sin, Cos, Exp, Ln
    };


    template<typename Type>
    class Node
    {
    public:
        virtual ~Node() = default;

        virtual std::unique_ptr<Node> makeCopy() = 0;

        virtual Priority getPriority() = 0;

        virtual TypeNode getType() = 0;

        virtual bool equal(std::unique_ptr<Node>& ptr) = 0;

        virtual std::string toString() = 0;

        virtual std::unique_ptr<Node> substitute(const std::string& variable, const std::unique_ptr<Node>& expression) = 0;

        virtual Type calculate(const std::vector<std::string>& variable, const std::vector<Type>& value) = 0;

        virtual std::unique_ptr<Node> differentiate(const std::string& variable) = 0;

        virtual std::unique_ptr<Node> simplify() = 0;
    };

    template<typename Type>
    class Number;

    template<typename Type>
    class Variable;

    template<typename Type>
    class Addition;

    template<typename Type>
    class Subtraction;

    template<typename Type>
    class Multiplication;

    template<typename Type>
    class Division;

    template<typename Type>
    class Minus;

    template<typename Type>
    class Power;

    template<typename Type>
    class Sin;

    template<typename Type>
    class Cos;

    template<typename Type>
    class Exp;

    template<typename Type>
    class Ln;
} // Math


#endif // NODE_HPP
