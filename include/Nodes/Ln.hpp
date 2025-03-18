#ifndef LN_HPP
#define LN_HPP


#include <complex>
#include <cmath>

#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Ln final : public Node<Type>
    {
    public:
        Ln(const Ln& ln);
        explicit Ln(const std::unique_ptr<Node<Type>>& argument);

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
    Ln<Type>::Ln(const Ln& ln)
    {
        this->argument = ln.argument->makeCopy();
    }


    template<typename Type>
    Ln<Type>::Ln(const std::unique_ptr<Node<Type>>& argument)
    {
        this->argument = argument->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Ln<Type>::makeCopy()
    {
        return std::make_unique<Ln>(this->argument);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Ln<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Ln>(this->argument->substitute(variable, expression));
    }


    template<typename Type>
    Type Ln<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        Type result = this->argument->calculate(variable, value);
        if(result == Type{})
        {
            throw std::invalid_argument("Logarithm from zero");
        }
        return std::log(result);
    }


    template<typename Type>
    Priority Ln<Type>::getPriority()
    {
        return Priority::Ln;
    }


    template<typename Type>
    TypeNode Ln<Type>::getType()
    {
        return TypeNode::Ln;
    }


    template<typename Type>
    bool Ln<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            Ln* ln = dynamic_cast<Ln*>(ptr.get());
            return this->argument->equal(ln->argument);
        }
        return false;
    }


    template<typename Type>
    std::string Ln<Type>::toString()
    {
        return "ln(" + this->argument->toString() + ")";
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Ln<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Multiplication<Type>>(
            std::make_unique<Division<Type>>(
                std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                this->argument
            ),
            this->argument->differentiate(variable)
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Ln<Type>::simplify()
    {
        this->argument = this->argument->simplify();

        // ln(a) = ln(a)
        if(this->argument->getType() == TypeNode::Number)
        {
            auto number = std::make_unique<Number<Type>>(this->calculate({}, {}));
            std::string result = number->toString();
            if(result == "0" || result == "-0" || result == "1" || result == "-1")
            {
                return number;
            }
        }

        // ln(exp(x)) = x
        if(this->argument->getType() == TypeNode::Exp)
        {
            auto* arg = dynamic_cast<Exp<Type>*>(this->argument.get());
            return arg->argument->makeCopy();
        }

        if(this->argument->getType() == TypeNode::Multiplication)
        {
            auto* arg = dynamic_cast<Multiplication<Type>*>(this->argument.get());
            // ln(exp(x) * a) = x + ln(a)
            if(arg->left->getType() == TypeNode::Exp)
            {
                auto* left = dynamic_cast<Exp<Type>*>(arg->left.get());
                return std::make_unique<Addition<Type>>(
                    left->argument,
                    std::make_unique<Ln>(
                        arg->right
                    )
                )->simplify();
            }
            // ln(a * exp(x)) = x + ln(a)
            if(arg->right->getType() == TypeNode::Exp)
            {
                auto* right = dynamic_cast<Exp<Type>*>(arg->right.get());
                return std::make_unique<Addition<Type>>(
                    right->argument,
                    std::make_unique<Ln>(
                        arg->left
                    )
                )->simplify();
            }
        }

        if(this->argument->getType() == TypeNode::Division)
        {
            auto* arg = dynamic_cast<Division<Type>*>(this->argument.get());
            // ln(exp(x) / a) = x - ln(a)
            if(arg->left->getType() == TypeNode::Exp)
            {
                auto* left = dynamic_cast<Exp<Type>*>(arg->left.get());
                return std::make_unique<Subtraction<Type>>(
                    left->argument,
                    std::make_unique<Ln>(
                        arg->right
                    )
                )->simplify();
            }
            // ln(a / exp(x)) = ln(a) - x
            if(arg->right->getType() == TypeNode::Exp)
            {
                auto* right = dynamic_cast<Exp<Type>*>(arg->right.get());
                return std::make_unique<Subtraction<Type>>(
                    std::make_unique<Ln>(
                        arg->left
                    ),
                    right->argument
                )->simplify();
            }
        }

        return std::make_unique<Ln>(this->argument);
    }
} // Math


#endif // LN_HPP
