#ifndef SUBTRACTION_HPP
#define SUBTRACTION_HPP


#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Subtraction final : public Node<Type>
    {
    public:
        Subtraction(const Subtraction& subtraction);
        Subtraction(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right);

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
    Subtraction<Type>::Subtraction(const Subtraction& subtraction)
    {
        this->left = subtraction.left->makeCopy();
        this->right = subtraction.right->makeCopy();
    }


    template<typename Type>
    Subtraction<Type>::Subtraction(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right)
    {
        this->left = left->makeCopy();
        this->right = right->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Subtraction<Type>::makeCopy()
    {
        return std::make_unique<Subtraction>(this->left, this->right);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Subtraction<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Subtraction>(this->left->substitute(variable, expression), this->right->substitute(variable, expression));
    }


    template<typename Type>
    Type Subtraction<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return this->left->calculate(variable, value) - this->right->calculate(variable, value);
    }


    template<typename Type>
    Priority Subtraction<Type>::getPriority()
    {
        return Priority::Subtraction;
    }


    template<typename Type>
    TypeNode Subtraction<Type>::getType()
    {
        return TypeNode::Subtraction;
    }


    template<typename Type>
    bool Subtraction<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* subtraction = dynamic_cast<Subtraction*>(ptr.get());
            return this->left->equal(subtraction->left) && this->right->equal(subtraction->right);
        }
        return false;
    }


    template<typename Type>
    std::string Subtraction<Type>::toString()
    {
        std::string left = this->left->toString();
        std::string right = this->right->toString();
        if(right.front() == '-' || this->right->getPriority() <= this->getPriority())
        {
            right = "(" + right + ")";
        }
        return left + " - " + right;
    }


    template<typename Type>
    std::unique_ptr<Node<Type> > Subtraction<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Subtraction<Type>>(
            this->left->differentiate(variable),
            this->right->differentiate(variable)
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Subtraction<Type>::simplify()
    {
        this->left = this->left->simplify();
        this->right = this->right->simplify();

        // 0 - x = -x
        if(this->left->toString() == "0" || this->left->toString() == "-0")
        {
            return std::make_unique<Minus<Type>>(this->right);
        }
        // x - 0 = x
        if(this->right->toString() == "0" || this->right->toString() == "-0")
        {
            return this->left->makeCopy();
        }
        // a - b = a - b
        if(this->left->getType() == TypeNode::Number && this->right->getType() == TypeNode::Number)
        {
            return std::make_unique<Number<Type>>(this->calculate({}, {}));
        }
        // x - x = 0
        if(this->left->equal(this->right))
        {
            return std::make_unique<Number<Type>>(Type{});
        }

        // -x - y = -(x + y)
        if(this->left->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Addition<Type>>(
                    left->argument,
                    this->right
                )
            )->simplify();
        }
        // x - (-y) = x + y
        if(this->right->getType() == TypeNode::Minus)
        {
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            return std::make_unique<Addition<Type>>(
                this->left,
                right->argument
            )->simplify();
        }

        if(this->right->getType() == TypeNode::Number)
        {
            if(this->left->getType() == TypeNode::Addition)
            {
                auto* left = dynamic_cast<Addition<Type>*>(this->left.get());
                // (x + a) - b = (a - b) + x
                if(left->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition<Type>>(
                        std::make_unique<Subtraction>(
                            left->right,
                            this->right
                        ),
                        left->left
                    )->simplify();
                }
                // (a + x) - b = (a - b) + x
                if(left->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition<Type>>(
                        std::make_unique<Subtraction>(
                            left->left,
                            this->right
                        ),
                        left->right
                    )->simplify();
                }
            }
            if(this->left->getType() == TypeNode::Subtraction)
            {
                auto* left = dynamic_cast<Subtraction*>(this->left.get());
                // (x - a) - b = x - (a + b)
                if(left->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction>(
                        left->left,
                        std::make_unique<Addition<Type>>(
                            left->right,
                            this->right
                        )
                    )->simplify();
                }
                // (a - x) - b = (a - b) - x
                if(left->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction>(
                        std::make_unique<Subtraction>(
                            left->left,
                            this->right
                        ),
                        left->right
                    )->simplify();
                }
            }
        }

        if(this->left->getType() == TypeNode::Number)
        {
            if(this->right->getType() == TypeNode::Addition)
            {
                auto* right = dynamic_cast<Addition<Type>*>(this->right.get());
                // a - (x + b) = (a - b) - x
                if(right->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction>(
                        std::make_unique<Subtraction>(
                            this->left,
                            right->right
                        ),
                        right->left
                    )->simplify();
                }
                // a - (b + x) = (a - b) - x
                if(right->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction>(
                        std::make_unique<Subtraction>(
                            this->left,
                            right->left
                        ),
                        right->right
                    )->simplify();
                }
            }
            if(this->right->getType() == TypeNode::Subtraction)
            {
                auto* right = dynamic_cast<Subtraction*>(this->right.get());
                // a - (x - b) = (a + b) - x
                if(right->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction>(
                        std::make_unique<Addition<Type>>(
                            this->left,
                            right->right
                        ),
                        right->left
                    )->simplify();
                }
                // a - (b - x) = (a - b) + x
                if(right->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition<Type>>(
                        std::make_unique<Subtraction>(
                            this->left,
                            right->left
                        ),
                        right->right
                    )->simplify();
                }
            }
        }

        if(this->left->getType() == TypeNode::Multiplication
           && this->right->getType() == TypeNode::Multiplication)
        {
            auto* left = dynamic_cast<Multiplication<Type>*>(this->left.get());
            auto* right = dynamic_cast<Multiplication<Type>*>(this->right.get());
            // x * a - x * b = (a - b) * x
            if(left->left->equal(right->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        left->right,
                        right->right
                    ),
                    left->left
                )->simplify();
            }
            // x * a - b * x = (a - b) * x
            if(left->left->equal(right->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        left->right,
                        right->left
                    ),
                    left->left
                )->simplify();
            }
            // a * x - x * b = (a - b) * x
            if(left->right->equal(right->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        left->left,
                        right->right
                    ),
                    left->right
                )->simplify();
            }
            // a * x - b * x = (a + b) * x
            if(left->right->equal(right->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        left->left,
                        right->left
                    ),
                    left->right
                )->simplify();
            }
        }

        if(this->left->getType() == TypeNode::Multiplication)
        {
            auto* left = dynamic_cast<Multiplication<Type>*>(this->left.get());
            // a * x - x = (a - 1) * x
            if(this->right->equal(left->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        left->left,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    ),
                    this->right
                )->simplify();
            }
            // x * a - x = (a - 1) * x
            if(this->right->equal(left->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        left->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    ),
                    this->right
                )->simplify();
            }
        }

        if(this->right->getType() == TypeNode::Multiplication)
        {
            auto* right = dynamic_cast<Multiplication<Type>*>(this->right.get());
            // x - a * x = (1 - a) * x
            if(this->left->equal(right->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                        right->left
                    ),
                    this->left
                )->simplify();
            }
            // x - x * a = (1 - a) * x
            if(this->left->equal(right->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Subtraction>(
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                        right->right
                    ),
                    this->left
                )->simplify();
            }
        }

        if(this->left->getType() == TypeNode::Division
           && this->right->getType() == TypeNode::Division)
        {
            auto* left = dynamic_cast<Division<Type>*>(this->left.get());
            auto* right = dynamic_cast<Division<Type>*>(this->right.get());
            // a / x - b / x = (a - b) / x
            if(left->right->equal(right->right))
            {
                return std::make_unique<Division<Type>>(
                    std::make_unique<Subtraction>(
                        left->left,
                        right->left
                    ),
                    left->right
                )->simplify();
            }
        }

        return std::make_unique<Subtraction>(this->left, this->right);
    }
} // Math


#endif // SUBTRACTION_HPP
