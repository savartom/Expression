#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP


#include <utility>
#include <memory>
#include <string>

#include "Parser.hpp"
#include "Nodes/Node.hpp"
#include "Nodes/Number.hpp"
#include "Nodes/Minus.hpp"
#include "Nodes/Addition.hpp"
#include "Nodes/Subtraction.hpp"
#include "Nodes/Sin.hpp"
#include "Nodes/Cos.hpp"
#include "Nodes/Exp.hpp"
#include "Nodes/Ln.hpp"
#include "Nodes/Multiplication.hpp"
#include "Nodes/Division.hpp"
#include "Nodes/Power.hpp"


namespace Math
{
    template<typename Type>
    class Expression;

    template<typename Type>
    Expression<Type> pow(const Expression<Type>& first, const Expression<Type>& second);

    template<typename Type>
    Expression<Type> sin(const Expression<Type>& expression);


    template<typename Type>
    Expression<Type> cos(const Expression<Type>& expression);


    template<typename Type>
    Expression<Type> exp(const Expression<Type>& expression);


    template<typename Type>
    Expression<Type> ln(const Expression<Type>& expression);
    

    template<typename Type>
    class Expression
    {
    public:
        Expression() = default;
        explicit Expression(Type number);
        explicit Expression(const std::string& expression);
        explicit Expression(const std::unique_ptr<Node<Type>>& node);

        ~Expression() = default;
        Expression(Expression& expression);
        Expression(Expression&& expression) noexcept = default;
        Expression& operator=(const Expression& expression);
        Expression& operator=(Expression&& expression) noexcept = default;

        Expression operator-();
        Expression operator+(const Expression& other);
        Expression operator-(const Expression& other);
        Expression operator*(const Expression& other);
        Expression operator/(const Expression& other);
        Expression operator^(const Expression& other);

        friend Expression pow<>(const Expression& first, const Expression& second);
        friend Expression sin<>(const Expression& expression);
        friend Expression cos<>(const Expression& expression);
        friend Expression exp<>(const Expression& expression);
        friend Expression ln<>(const Expression& expression);

        [[nodiscard]] std::string toString() const;

        Expression substitute(const std::string& variable, const Expression& expression);

        Type calculate(const std::vector<std::string>& variable, const std::vector<Type>& value);

        Expression differentiate(const std::string& variable="x", int number=1);

        Expression simplify();

    private:
        std::unique_ptr<Node<Type>> root;
    };
} // Math



// Implementation
namespace Math
{
    template<typename Type>
    Expression<Type>::Expression(Expression &expression)
    {
        this->root = expression.root->makeCopy();
    }


    template<typename Type>
    Expression<Type>::Expression(const std::unique_ptr<Node<Type>>& node)
    {
        this->root = node->makeCopy();
    }


    template<typename Type>
    Expression<Type>& Expression<Type>::operator=(const Expression &expression)
    {
        this->root = expression.root->makeCopy();
        return *this;
    }


    template<typename Type>
    Expression<Type>::Expression(Type number)
    {
        this->root = std::make_unique<Number<Type>>(number);
    }


    template<typename Type>
    Expression<Type>::Expression(const std::string& expression)
    {
        this->root = Parser<Type>(expression).parseExpression();
    }


    template<typename Type>
    Expression<Type> Expression<Type>::operator-()
    {
        Expression newExpression;
        newExpression.root = std::make_unique<Minus<Type>>(this->root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> Expression<Type>::operator+(const Expression &other)
    {
        Expression newExpression;
        newExpression.root = std::make_unique<Addition<Type>>(this->root, other.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> Expression<Type>::operator-(const Expression &other)
    {
        Expression newExpression;
        newExpression.root = std::make_unique<Subtraction<Type>>(this->root, other.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> Expression<Type>::operator*(const Expression &other)
    {
        Expression newExpression;
        newExpression.root = std::make_unique<Multiplication<Type>>(this->root, other.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> Expression<Type>::operator/(const Expression &other)
    {
        Expression newExpression;
        newExpression.root = std::make_unique<Division<Type>>(this->root, other.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> Expression<Type>::operator^(const Expression &other)
    {
        Expression newExpression;
        newExpression.root = std::make_unique<Power<Type>>(this->root, other.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> pow(const Expression<Type>& first, const Expression<Type>& second)
    {
        Expression<Type> newExpression;
        newExpression.root = std::make_unique<Power<Type>>(first.root, second.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> sin(const Expression<Type>& expression)
    {
        Expression<Type> newExpression;
        newExpression.root = std::make_unique<Sin<Type>>(expression.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> cos(const Expression<Type>& expression)
    {
        Expression<Type> newExpression;
        newExpression.root = std::make_unique<Cos<Type>>(expression.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> exp(const Expression<Type>& expression)
    {
        Expression<Type> newExpression;
        newExpression.root = std::make_unique<Exp<Type>>(expression.root);
        return newExpression;
    }


    template<typename Type>
    Expression<Type> ln(const Expression<Type>& expression)
    {
        Expression<Type> newExpression;
        newExpression.root = std::make_unique<Ln<Type>>(expression.root);
        return newExpression;
    }


    template<typename Type>
    std::string Expression<Type>::toString() const
    {
        return this->root->toString();
    }


    template<typename Type>
    Expression<Type> Expression<Type>::substitute(const std::string& variable, const Expression& expression)
    {
        Expression newExpression(*this);
        newExpression.root = newExpression.root->substitute(variable, expression.root)->simplify();
        return newExpression;
    }


    template<typename Type>
    Type Expression<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return this->root->calculate(variable, value);
    }


    template<typename Type>
    Expression<Type> Expression<Type>::simplify()
    {
        Expression newExpression(*this);
        newExpression.root = newExpression.root->simplify();
        return newExpression;
    }


    template<typename Type>
    Expression<Type> Expression<Type>::differentiate(const std::string &variable, int number)
    {
        Expression derivative(*this);
        for(int i = 0; i < number; i++)
        {
            derivative.root = derivative.root->simplify()->differentiate(variable);
        }
        return derivative.simplify();
    }


    template<typename Type>
    std::ostream& operator<<(std::ostream& ostream, const Expression<Type>& expression)
    {
        ostream << expression.toString();
        return ostream;
    }
} // Math


#endif // EXPRESSION_HPP
