#ifndef POWER_HPP
#define POWER_HPP


#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Power final : public Node<Type>
    {
    public:
        Power(const Power& power);
        Power(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right);

        std::unique_ptr<Node<Type>> makeCopy() override;

        Priority getPriority() override;

        TypeNode getType() override;

        bool equal(std::unique_ptr<Node<Type>>& ptr) override;

        std::string toString() override;

        std::unique_ptr<Node<Type>> substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression) override;

        Type calculate(const std::vector<std::string>& variable, const std::vector<Type>& value) override;

        std::unique_ptr<Node<Type>> differentiate(const std::string& variable) override;

        std::unique_ptr<Node<Type>> simplify() override;

        std::unique_ptr<Node<Type>> left;
        std::unique_ptr<Node<Type>> right;
    };
} // Math


namespace Math
{
    template<typename Type>
    Power<Type>::Power(const Power& power)
    {
        this->left = power.left->makeCopy();
        this->right = power.right->makeCopy();
    }


    template<typename Type>
    Power<Type>::Power(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right)
    {
        this->left = left->makeCopy();
        this->right = right->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Power<Type>::makeCopy()
    {
        return std::make_unique<Power>(this->left, this->right);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Power<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Power>(this->left->substitute(variable, expression), this->right->substitute(variable, expression));
    }


    template<typename Type>
    Type Power<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return std::pow(this->left->calculate(variable, value), this->right->calculate(variable, value));
    }


    template<typename Type>
    Priority Power<Type>::getPriority()
    {
        return Priority::Power;
    }


    template<typename Type>
    TypeNode Power<Type>::getType()
    {
        return TypeNode::Power;
    }


    template<typename Type>
    bool Power<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* power = dynamic_cast<Power*>(ptr.get());
            return this->left->equal(power->left) && this->right->equal(power->right);
        }
        return false;
    }


    template<typename Type>
    std::string Power<Type>::toString()
    {
        std::string left = this->left->toString();
        std::string right = this->right->toString();
        if(left.front() == '-' || this->left->getPriority() <= this->getPriority())
        {
            left = "(" + left + ")";
        }
        if(right.front() == '-' || this->right->getPriority() <= this->getPriority())
        {
            right = "(" + right + ")";
        }
        return left + "^" + right;
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Power<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Multiplication<Type>>(
            std::make_unique<Addition<Type>>(
                std::make_unique<Division<Type>>(
                    std::make_unique<Multiplication<Type>>(
                        this->right,
                        this->left->differentiate(variable)
                    ),
                    this->left
                ),
                std::make_unique<Multiplication<Type>>(
                    this->right->differentiate(variable),
                    std::make_unique<Ln<Type>>(this->left)
                )
            ),
            this->makeCopy()
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Power<Type>::simplify()
    {
        this->left = this->left->simplify();
        this->right = this->right->simplify();

        // 0^x = 0
        if((this->left->toString() == "0" || this->left->toString() == "-0")
           && !(this->right->toString() == "0" || this->right->toString() == "-0"))
        {
            return this->left->makeCopy();
        }
        // 1^x = 1
        if(this->left->toString() == "1")
        {
            return this->left->makeCopy();
        }
        // x^1 = x
        if(this->right->toString() == "1")
        {
            return this->left->makeCopy();
        }
        // x^0 = 1
        if(this->right->toString() == "0" || this->right->toString() == "-0")
        {
            return std::make_unique<Number<Type>>(getNumber<Type>(1.0));
        }

        if(this->left->getType() == TypeNode::Power)
        {
            auto* left = dynamic_cast<Power*>(this->left.get());
            return std::make_unique<Power>(
                left->left,
                std::make_unique<Multiplication<Type>>(
                    left->right,
                    this->right
                )
            )->simplify();
        }

        return std::make_unique<Power>(this->left, this->right);
    }
} // Math


#endif // POWER_HPP
