#ifndef ADDITION_HPP
#define ADDITION_HPP


#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Addition final : public Node<Type>
    {
    public:
        Addition(const Addition& addition);
        Addition(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right);

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
    Addition<Type>::Addition(const Addition& addition)
    {
        this->left = addition.left->makeCopy();
        this->right = addition.right->makeCopy();
    }


    template<typename Type>
    Addition<Type>::Addition(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right)
    {
        this->left = left->makeCopy();
        this->right = right->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Addition<Type>::makeCopy()
    {
        return std::make_unique<Addition>(this->left, this->right);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Addition<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Addition>(this->left->substitute(variable, expression), this->right->substitute(variable, expression));
    }


    template<typename Type>
    Type Addition<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return this->left->calculate(variable, value) + this->right->calculate(variable, value);
    }


    template<typename Type>
    Priority Addition<Type>::getPriority()
    {
        return Priority::Addition;
    }


    template<typename Type>
    TypeNode Addition<Type>::getType()
    {
        return TypeNode::Addition;
    }


    template<typename Type>
    bool Addition<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* addition = dynamic_cast<Addition*>(ptr.get());
            return this->left->equal(addition->left) && this->right->equal(addition->right);
        }
        return false;
    }


    template<typename Type>
    std::string Addition<Type>::toString()
    {
        std::string left = this->left->toString();
        std::string right = this->right->toString();
        if(right.front() == '-')
        {
            return left + " + (" + right + ")";
        }
        return left + " + " + right;
    }


    template<typename Type>
    std::unique_ptr<Node<Type> > Addition<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Addition>(
            this->left->differentiate(variable),
            this->right->differentiate(variable)
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Addition<Type>::simplify()
    {
        this->left = this->left->simplify();
        this->right = this->right->simplify();

        // 0 + x = x
        if(this->left->toString() == "0" || this->left->toString() == "-0")
        {
            return this->right->makeCopy();
        }
        // x + 0 = x
        if(this->right->toString() == "0" || this->right->toString() == "-0")
        {
            return this->left->makeCopy();
        }
        // a + b = a + b
        if(this->left->getType() == TypeNode::Number && this->right->getType() == TypeNode::Number)
        {
            return std::make_unique<Number<Type>>(this->calculate({}, {}));
        }

        // -x + (-y) = -(x + y)
        if(this->left->getType() == TypeNode::Minus
           && this->right->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Addition>(
                    left->argument,
                    right->argument
                )
            )->simplify();
        }
        if(this->left->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            // -x + y = y - x
            return std::make_unique<Subtraction<Type>>(
                this->right,
                left->argument
            )->simplify();
        }
        if(this->right->getType() == TypeNode::Minus)
        {
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            // x + (-y) = x - y
            return std::make_unique<Subtraction<Type>>(
                this->left,
                right->argument
            )->simplify();
        }

        // x + x = 2 * x
        if(this->left->equal(this->right))
        {
            return std::make_unique<Multiplication<Type>>(
                std::make_unique<Number<Type>>(getNumber<Type>(2.0)),
                this->left
            );
        }

        // sin(x)^2 + cos(x)^2 = 1
        if(this->left->getType() == TypeNode::Power
           && this->right->getType() == TypeNode::Power)
        {
            std::unique_ptr<Node<Type>> two = std::make_unique<Number<Type>>(getNumber<Type>(2.0));
            auto* left = dynamic_cast<Power<Type>*>(this->left.get());
            auto* right = dynamic_cast<Power<Type>*>(this->right.get());
            if(left->right->equal(two) && right->right->equal(two))
            {
                if(left->left->getType() == TypeNode::Sin && right->left->getType() == TypeNode::Cos)
                {
                    auto* sin = dynamic_cast<Sin<Type>*>(left->left.get());
                    auto* cos = dynamic_cast<Cos<Type>*>(right->left.get());
                    if(sin->argument->equal(cos->argument))
                    {
                        return std::make_unique<Number<Type>>(getNumber<Type>(1.0));
                    }
                }
                if(left->left->getType() == TypeNode::Cos && right->left->getType() == TypeNode::Sin)
                {
                    auto* sin = dynamic_cast<Sin<Type>*>(right->left.get());
                    auto* cos = dynamic_cast<Cos<Type>*>(left->left.get());
                    if(sin->argument->equal(cos->argument))
                    {
                        return std::make_unique<Number<Type>>(getNumber<Type>(1.0));
                    }
                }
            }
        }

        if(this->left->getType() == TypeNode::Number)
        {
            if(this->right->getType() == TypeNode::Addition)
            {
                auto* right = dynamic_cast<Addition*>(this->right.get());
                // a + (b + x) = (a + b) + x
                if(right->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition>(
                        std::make_unique<Addition>(
                            right->left,
                            this->left
                        ),
                        right->right
                    );
                }
                // a + (x + b) = (a + b) + x
                if(right->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition>(
                        std::make_unique<Addition>(
                            right->right,
                            this->left
                        ),
                        right->left
                    );
                }
            }
            if(this->right->getType() == TypeNode::Subtraction)
            {
                auto* right = dynamic_cast<Subtraction<Type>*>(this->right.get());
                // a + (b - x) = (a + b) - x
                if(right->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction<Type>>(
                        std::make_unique<Addition>(
                            right->left,
                            this->left
                        ),
                        right->right
                    );
                }
                // a + (x - b) = (a - b) + x
                if(right->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition>(
                        std::make_unique<Subtraction<Type>>(
                            this->left,
                            right->right
                        ),
                        right->left
                    );
                }
            }
        }

        if(this->right->getType() == TypeNode::Number)
        {
            if(this->left->getType() == TypeNode::Addition)
            {
                auto* left = dynamic_cast<Addition*>(this->left.get());
                // (a + x) + b = (a + b) + x
                if(left->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition>(
                        std::make_unique<Addition>(
                            left->left,
                            this->right
                        ),
                        left->right
                    );
                }
                // (x + a) + b = (a + b) + x
                if(left->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition>(
                        std::make_unique<Addition>(
                            left->right,
                            this->right
                        ),
                        left->left
                    );
                }
            }
            if(this->left->getType() == TypeNode::Subtraction)
            {
                auto* left = dynamic_cast<Subtraction<Type>*>(this->left.get());
                // (a - x) + b = (a + b) - x
                if(left->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Subtraction<Type>>(
                        std::make_unique<Addition>(
                            left->left,
                            this->right
                        ),
                        left->right
                    );
                }
                // (x - a) + b = (b - a) + x
                if(left->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Addition>(
                        std::make_unique<Subtraction<Type>>(
                            this->right,
                            left->right
                        ),
                        left->left
                    );
                }
            }
        }

        if(this->left->getType() == TypeNode::Multiplication
           && this->right->getType() == TypeNode::Multiplication)
        {
            auto* left = dynamic_cast<Multiplication<Type>*>(this->left.get());
            auto* right = dynamic_cast<Multiplication<Type>*>(this->right.get());
            // x * a + x * b = (a + b) * x
            if(left->left->equal(right->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
                        left->right,
                        right->right
                    ),
                    left->left
                )->simplify();
            }
            // x * a + b * x = (a + b) * x
            if(left->left->equal(right->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
                        left->right,
                        right->left
                    ),
                    left->left
                )->simplify();
            }
            // a * x + x * b = (a + b) * x
            if(left->right->equal(right->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
                        left->left,
                        right->right
                    ),
                    left->right
                )->simplify();
            }
            // a * x + b * x = (a + b) * x
            if(left->right->equal(right->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
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
            // a * x + x = (a + 1) * x
            if(this->right->equal(left->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
                        left->left,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    ),
                    this->right
                )->simplify();
            }
            // x * a + x = (a + 1) * x
            if(this->right->equal(left->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
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
            // x + a * x = (a + 1) * x
            if(this->left->equal(right->right))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
                        right->left,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    ),
                    this->left
                )->simplify();
            }
            // x + x * a = (a + 1) * x
            if(this->left->equal(right->left))
            {
                return std::make_unique<Multiplication<Type>>(
                    std::make_unique<Addition>(
                        right->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
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
            // a / x + b / x = (a + b) / x
            if(left->right->equal(right->right))
            {
                return std::make_unique<Division<Type>>(
                    std::make_unique<Addition>(
                        left->left,
                        right->left
                    ),
                    left->right
                )->simplify();
            }
        }

        return std::make_unique<Addition>(this->left, this->right);
    }
} // Math


#endif // ADDITION_HPP
