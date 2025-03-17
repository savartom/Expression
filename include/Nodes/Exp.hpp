#ifndef EXP_HPP
#define EXP_HPP


#include <complex>
#include <cmath>

#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Exp final : public Node<Type>
    {
    public:
        Exp(const Exp& exp);
        explicit Exp(const std::unique_ptr<Node<Type>>& argument);

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
    Exp<Type>::Exp(const Exp& exp)
    {
        this->argument = exp.argument->makeCopy();
    }


    template<typename Type>
    Exp<Type>::Exp(const std::unique_ptr<Node<Type>>& argument)
    {
        this->argument = argument->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Exp<Type>::makeCopy()
    {
        return std::make_unique<Exp>(this->argument);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Exp<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Exp>(this->argument->substitute(variable, expression));
    }


    template<typename Type>
    Type Exp<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return std::exp(this->argument->calculate(variable, value));
    }


    template<typename Type>
    Priority Exp<Type>::getPriority()
    {
        return Priority::Exp;
    }


    template<typename Type>
    TypeNode Exp<Type>::getType()
    {
        return TypeNode::Exp;
    }


    template<typename Type>
    bool Exp<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            Exp* exp = dynamic_cast<Exp*>(ptr.get());
            return this->argument->equal(exp->argument);
        }
        return false;
    }


    template<typename Type>
    std::string Exp<Type>::toString()
    {
        return "exp(" + this->argument->toString() + ")";
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Exp<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Multiplication<Type>>(
            std::make_unique<Exp<Type>>(this->argument),
            this->argument->differentiate(variable)
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Exp<Type>::simplify()
    {
        this->argument = this->argument->simplify();

        // exp(a) = exp(a)
        if(this->argument->getType() == TypeNode::Number)
        {
            auto number = std::make_unique<Number<Type>>(this->calculate({}, {}));
            std::string result = number->toString();
            if(result == "0" || result == "-0" || result == "1" || result == "-1")
            {
                return number;
            }
        }

        // exp(ln(x)) = x
        if(this->argument->getType() == TypeNode::Ln)
        {
            auto* arg = dynamic_cast<Ln<Type>*>(this->argument.get());
            return arg->argument->makeCopy();
        }

        if(this->argument->getType() == TypeNode::Addition)
        {
            auto* arg = dynamic_cast<Addition<Type>*>(this->argument.get());

            // exp(ln(x) + y) = x * exp(y)
            if(arg->left->getType() == TypeNode::Ln)
            {
                auto* left = dynamic_cast<Ln<Type>*>(arg->left.get());
                return std::make_unique<Multiplication<Type>>(
                    left->argument,
                    std::make_unique<Exp>(
                        arg->right
                    )
                )->simplify();
            }
            // exp(y + ln(x)) = x * exp(y)
            if(arg->right->getType() == TypeNode::Ln)
            {
                auto* right = dynamic_cast<Ln<Type>*>(arg->right.get());
                return std::make_unique<Multiplication<Type>>(
                    right->argument,
                    std::make_unique<Exp>(
                        arg->left
                    )
                )->simplify();
            }
        }

        if(this->argument->getType() == TypeNode::Subtraction)
        {
            auto* arg = dynamic_cast<Subtraction<Type>*>(this->argument.get());

            // exp(ln(x) - y) = x / exp(y)
            if(arg->left->getType() == TypeNode::Ln)
            {
                auto* left = dynamic_cast<Ln<Type>*>(arg->left.get());
                return std::make_unique<Division<Type>>(
                    left->argument,
                    std::make_unique<Exp>(
                        arg->right
                    )
                )->simplify();
            }
            // exp(y - ln(x)) = exp(y) / x
            if(arg->right->getType() == TypeNode::Ln)
            {
                auto* right = dynamic_cast<Ln<Type>*>(arg->right.get());
                return std::make_unique<Division<Type>>(
                    std::make_unique<Exp>(
                        arg->left
                    ),
                    right->argument
                )->simplify();
            }
        }

        if(this->argument->getType() == TypeNode::Multiplication)
        {
            auto* arg = dynamic_cast<Multiplication<Type>*>(this->argument.get());
            // exp(ln(x) * a) = x^a
            if(arg->left->getType() == TypeNode::Ln)
            {
                auto* left = dynamic_cast<Ln<Type>*>(arg->left.get());
                return std::make_unique<Power<Type>>(
                    left->argument,
                    arg->right
                )->simplify();
            }
            // exp(a * ln(x)) = x^a
            if(arg->right->getType() == TypeNode::Ln)
            {
                auto* right = dynamic_cast<Ln<Type>*>(arg->right.get());
                return std::make_unique<Power<Type>>(
                    right->argument,
                    arg->left
                )->simplify();
            }
        }

        if(this->argument->getType() == TypeNode::Division)
        {
            auto* arg = dynamic_cast<Division<Type>*>(this->argument.get());
            // exp(ln(x) / a) = x^(1 / a)
            if(arg->left->getType() == TypeNode::Ln)
            {
                auto* left = dynamic_cast<Ln<Type>*>(arg->left.get());
                return std::make_unique<Power<Type>>(
                    left->argument,
                    std::make_unique<Division<Type>>(
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                        arg->right
                    )
                )->simplify();
            }
        }

        return std::make_unique<Exp>(this->argument);
    }
} // Math


#endif // EXP_HPP
