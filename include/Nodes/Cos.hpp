#ifndef COS_HPP
#define COS_HPP


#include <complex>
#include <cmath>

#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Cos final : public Node<Type>
    {
    public:
        Cos(const Cos& cos);
        explicit Cos(const std::unique_ptr<Node<Type>>& argument);

        std::unique_ptr<Node<Type>> makeCopy() override;

        Priority getPriority() override;

        TypeNode getType() override;

        bool equal(std::unique_ptr<Node<Type>>& ptr) override;

        std::string toString() override;

        std::unique_ptr<Node<Type>> substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression) override;

        Type calculate(const std::vector<std::string>& variable, const std::vector<Type>& value) override;

        std::unique_ptr<Node<Type>> differentiate(const std::string& variable) override;

        std::unique_ptr<Node<Type>> simplify() override;

        std::unique_ptr<Node<Type>> argument;
    };
} // Math


namespace Math
{
    template<typename Type>
    Cos<Type>::Cos(const Cos& cos)
    {
        this->argument = cos.argument->makeCopy();
    }


    template<typename Type>
    Cos<Type>::Cos(const std::unique_ptr<Node<Type>>& argument)
    {
        this->argument = argument->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Cos<Type>::makeCopy()
    {
        return std::make_unique<Cos>(this->argument);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Cos<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Cos>(this->argument->substitute(variable, expression));
    }


    template<typename Type>
    Type Cos<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return std::cos(this->argument->calculate(variable, value));
    }


    template<typename Type>
    Priority Cos<Type>::getPriority()
    {
        return Priority::Cos;
    }


    template<typename Type>
    TypeNode Cos<Type>::getType()
    {
        return TypeNode::Cos;
    }


    template<typename Type>
    bool Cos<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            Cos* cos = dynamic_cast<Cos*>(ptr.get());
            return this->argument->equal(cos->argument);
        }
        return false;
    }


    template<typename Type>
    std::string Cos<Type>::toString()
    {
        return "cos(" + this->argument->toString() + ")";
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Cos<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Multiplication<Type>>(
            std::make_unique<Minus<Type>>(
                std::make_unique<Sin<Type>>(this->argument)
            ),
            this->argument->differentiate(variable)
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Cos<Type>::simplify()
    {
        this->argument = this->argument->simplify();

        // cos(a) = cos(a)
        if(this->argument->getType() == TypeNode::Number)
        {
            auto number = std::make_unique<Number<Type>>(this->calculate({}, {}));
            std::string result = number->toString();
            if(result == "0" || result == "-0" || result == "1" || result == "-1")
            {
                return number;
            }
        }

        // cos(-x) = cos(x)
        if(this->argument->getType() == TypeNode::Minus)
        {
            auto* arg = dynamic_cast<Minus<Type>*>(this->argument.get());
            return std::make_unique<Cos>(arg->argument)->simplify();
        }

        return std::make_unique<Cos>(this->argument);
    }
} // Math


#endif // COS_HPP
