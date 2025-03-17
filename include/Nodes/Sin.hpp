#ifndef SIN_HPP
#define SIN_HPP


#include <complex>
#include <cmath>

#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Sin final : public Node<Type>
    {
    public:
        Sin(const Sin& sin);
        explicit Sin(const std::unique_ptr<Node<Type>>& argument);

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
    Sin<Type>::Sin(const Sin& sin)
    {
        this->argument = sin.argument->makeCopy();
    }


    template<typename Type>
    Sin<Type>::Sin(const std::unique_ptr<Node<Type>>& argument)
    {
        this->argument = argument->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Sin<Type>::makeCopy()
    {
        return std::make_unique<Sin>(this->argument);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Sin<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Sin>(this->argument->substitute(variable, expression));
    }


    template<typename Type>
    Type Sin<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return std::sin(this->argument->calculate(variable, value));
    }


    template<typename Type>
    Priority Sin<Type>::getPriority()
    {
        return Priority::Sin;
    }


    template<typename Type>
    TypeNode Sin<Type>::getType()
    {
        return TypeNode::Sin;
    }


    template<typename Type>
    bool Sin<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            Sin* sin = dynamic_cast<Sin*>(ptr.get());
            return this->argument->equal(sin->argument);
        }
        return false;
    }


    template<typename Type>
    std::string Sin<Type>::toString()
    {
        return "sin(" + this->argument->toString() + ")";
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Sin<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Multiplication<Type>>(
            std::make_unique<Cos<Type>>(this->argument),
            this->argument->differentiate(variable)
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Sin<Type>::simplify()
    {
        this->argument = this->argument->simplify();

        // sin(a) = sin(a)
        if(this->argument->getType() == TypeNode::Number)
        {
            auto number = std::make_unique<Number<Type>>(this->calculate({}, {}));
            std::string result = number->toString();
            if(result == "0" || result == "-0" || result == "1" || result == "-1")
            {
                return number;
            }
        }

        // sin(-x) = -sin(x)
        if(this->argument->getType() == TypeNode::Minus)
        {
            auto* arg = dynamic_cast<Minus<Type>*>(this->argument.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Sin>(
                    arg->argument
                )
            )->simplify();
        }

        return std::make_unique<Sin>(this->argument);
    }
} // Math


#endif // SIN_HPP
