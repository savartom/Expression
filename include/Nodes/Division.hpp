#ifndef DIVISION_HPP
#define DIVISION_HPP


#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Division final : public Node<Type>
    {
    public:
        Division(const Division& division);
        Division(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right);

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
    Division<Type>::Division(const Division& division)
    {
        this->left = division.left->makeCopy();
        this->right = division.right->makeCopy();
    }


    template<typename Type>
    Division<Type>::Division(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right)
    {
        this->left = left->makeCopy();
        this->right = right->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Division<Type>::makeCopy()
    {
        return std::make_unique<Division>(this->left, this->right);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Division<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Division>(this->left->substitute(variable, expression), this->right->substitute(variable, expression));
    }


    template<typename Type>
    Type Division<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return this->left->calculate(variable, value) / this->right->calculate(variable, value);
    }


    template<typename Type>
    Priority Division<Type>::getPriority()
    {
        return Priority::Division;
    }


    template<typename Type>
    TypeNode Division<Type>::getType()
    {
        return TypeNode::Division;
    }


    template<typename Type>
    bool Division<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* division = dynamic_cast<Division*>(ptr.get());
            return this->left->equal(division->left) && this->right->equal(division->right);
        }
        return false;
    }


    template<typename Type>
    std::string Division<Type>::toString()
    {
        std::string left = this->left->toString();
        std::string right = this->right->toString();
        if(this->left->getPriority() < this->getPriority())
        {
            left = "(" + left + ")";
        }
        if(right.front() == '-' || this->right->getPriority() <= this->getPriority())
        {
            right = "(" + right + ")";
        }
        return left + " / " + right;
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Division<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Division<Type>>(
            std::make_unique<Subtraction<Type>>(
                std::make_unique<Multiplication<Type>>(
                    this->left->differentiate(variable),
                    this->right
                ),
                std::make_unique<Multiplication<Type>>(
                    this->left,
                    this->right->differentiate(variable))
                ),
            std::make_unique<Power<Type>>(
                this->right,
                std::make_unique<Number<Type>>(getNumber<Type>(2.0))
            )
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Division<Type>::simplify()
    {
        this->left = this->left->simplify();
        this->right = this->right->simplify();

        // 0 / x = 0
        if(this->left->toString() == "0" || this->left->toString() == "-0")
        {
            return std::make_unique<Number<Type>>(Type{});
        }
        // x / 1 = x
        if(this->right->toString() == "1")
        {
            return this->left->makeCopy();
        }
        // x / -1 = -x
        if(this->right->toString() == "-1")
        {
            return std::make_unique<Minus<Type>>(this->left);
        }
        // x / x = 1
        if(this->left->equal(this->right))
        {
            return std::make_unique<Number<Type>>(getNumber<Type>(1.0));
        }

        // a / b = a / b
        if(this->left->getType() == TypeNode::Number && this->right->getType() == TypeNode::Number)
        {
            return std::make_unique<Number<Type>>(this->calculate({}, {}));
        }

        // -x / -y = x / y
        if(this->left->getType() == TypeNode::Minus && this->right->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            return std::make_unique<Division>(
                left->argument,
                right->argument
            )->simplify();
        }
        // -x / y = -(x / y)
        if(this->left->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Division>(
                    left->argument,
                    this->right
                )
            )->simplify();
        }
        // x / -y = -(x / y)
        if(this->right->getType() == TypeNode::Minus)
        {
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Division>(
                    this->left,
                    right->argument
                )
            )->simplify();
        }

        if(this->left->getType() == TypeNode::Division
           && this->right->getType() == TypeNode::Division)
        {
            auto* left = dynamic_cast<Division*>(this->left.get());
            auto* right = dynamic_cast<Division*>(this->right.get());
            // (x / a) / (y / b) = (x * b) / (a * y)
            return std::make_unique<Division>(
                std::make_unique<Multiplication<Type>>(
                    left->left,
                    right->right
                ),
                std::make_unique<Multiplication<Type>>(
                    left->right,
                    right->left
                )
            )->simplify();
        }

        // (a / b) / x = a / (b * x)
        if(this->left->getType() == TypeNode::Division)
        {
            auto* left = dynamic_cast<Division*>(this->left.get());
            return std::make_unique<Division>(
                left->left,
                std::make_unique<Multiplication<Type>>(
                    left->right,
                    this->right
                )
            )->simplify();
        }
        // x / (a / b) = x * b / a
        if(this->right->getType() == TypeNode::Division)
        {
            auto* right = dynamic_cast<Division*>(this->right.get());
            return std::make_unique<Division>(
                std::make_unique<Multiplication<Type>>(
                    this->left,
                    right->right
                ),
                right->left
            )->simplify();
        }

        if(this->left->getType() == TypeNode::Multiplication && this->right->getType() == TypeNode::Multiplication)
        {
            auto* left = dynamic_cast<Multiplication<Type>*>(this->left.get());
            auto* right = dynamic_cast<Multiplication<Type>*>(this->right.get());

            // (x * a) / (x * b) = a / b
            if(left->left->equal(right->left))
            {
                return std::make_unique<Division>(
                    left->right,
                    right->right
                )->simplify();
            }
            // (x * a) / (b * x) = a / b
            if(left->left->equal(right->right))
            {
                return std::make_unique<Division>(
                    left->right,
                    right->left
                )->simplify();
            }
            // (a * x) / (x * b) = a / b
            if(left->right->equal(right->left))
            {
                return std::make_unique<Division>(
                    left->left,
                    right->right
                )->simplify();
            }
            // (a * x) / (b * x) = a / b
            if(left->right->equal(right->right))
            {
                return std::make_unique<Division>(
                    left->left,
                    right->left
                )->simplify();
            }
        }

        if(this->left->getType() == TypeNode::Multiplication)
        {
            auto* left = dynamic_cast<Multiplication<Type>*>(this->left.get());
            // (x * a) / x = a
            if(left->left->equal(this->right))
            {
                return left->right->makeCopy();
            }
            // (a * x) / x = a
            if(left->right->equal(this->right))
            {
                return left->left->makeCopy();
            }
            if(left->left->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(left->left.get());
                // (x^b * a) / x = a * x^(b - 1)
                if(pow1->left->equal(this->right))
                {
                    return std::make_unique<Multiplication<Type>>(
                        left->right,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Subtraction<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                if(this->right->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->right.get());
                    // (x^b * a) / x^c = a * x^(b - c)
                    if(pow1->left->equal(pow2->left))
                    {
                        return std::make_unique<Multiplication<Type>>(
                            left->right,
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Subtraction<Type>>(
                                    pow1->right,
                                    pow2->right
                                )
                        )
                    )->simplify();
                    }
                }
            }
            if(left->right->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(left->right.get());
                // (a * x^b) / x = a * x^(b - 1)
                if(pow1->left->equal(this->right))
                {
                    return std::make_unique<Multiplication<Type>>(
                        left->left,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Subtraction<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                if(this->right->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->right.get());
                    // (a * x^b) / x^c = a * x^(b - c)
                    if(pow1->left->equal(pow2->left))
                    {
                        return std::make_unique<Multiplication<Type>>(
                            left->left,
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Subtraction<Type>>(
                                    pow1->right,
                                    pow2->right
                                )
                            )
                        )->simplify();
                    }
                }
            }
        }

        if(this->right->getType() == TypeNode::Multiplication)
        {
            auto* right = dynamic_cast<Multiplication<Type>*>(this->right.get());
            // x / (x * a) = 1 / a
            if(right->left->equal(this->left))
            {
                return std::make_unique<Division>(
                    std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                    right->right
                )->simplify();
            }
            // x / (a * x) = 1 / a
            if(right->right->equal(this->left))
            {
                return std::make_unique<Division>(
                    std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                    right->left
                )->simplify();
            }
            if(right->left->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(right->left.get());
                // x / (x^b * a) = x^(1 - b) / a
                if(pow1->left->equal(this->left))
                {
                    return std::make_unique<Division>(
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Subtraction<Type>>(
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                                pow1->right
                            )
                        ),
                        right->right
                    )->simplify();
                }
                if(this->left->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->left.get());
                    // x^c / (x^b * a) = x^(c - b) / a
                    if(pow1->left->equal(pow2->left))
                    {
                        return std::make_unique<Division>(
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Subtraction<Type>>(
                                    pow2->right,
                                    pow1->right
                                )
                            ),
                            right->right
                        )->simplify();
                    }
                }
            }
            if(right->right->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(right->right.get());
                // x / (a * x^b) = x^(1 - b) / a
                if(pow1->left->equal(this->left))
                {
                    return std::make_unique<Division>(
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Subtraction<Type>>(
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                                pow1->right
                            )
                        ),
                        right->left
                    )->simplify();
                }
                if(this->left->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->left.get());
                    // x^c / (a * x^b) = x^(c - b) / a
                    if(pow1->left->equal(pow2->left))
                    {
                        return std::make_unique<Division>(
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Subtraction<Type>>(
                                    pow2->right,
                                    pow1->right
                                )
                            ),
                            right->left
                        )->simplify();
                    }
                }
            }
        }

        if(this->left->getType() == TypeNode::Power)
        {
            auto* left = dynamic_cast<Power<Type>*>(this->left.get());
            // x^a / x = x^(a - 1)
            if(left->left->equal(this->right))
            {
                return std::make_unique<Power<Type>>(
                    left->left,
                    std::make_unique<Subtraction<Type>>(
                        left->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    )
                )->simplify();
            }
            if(this->right->getType() == TypeNode::Power)
            {
                auto* right = dynamic_cast<Power<Type>*>(this->right.get());
                // x^a / x^b = x^(a - b)
                if(left->left->equal(right->left))
                {
                    return std::make_unique<Power<Type>>(
                        left->left,
                        std::make_unique<Subtraction<Type>>(
                            left->right,
                            right->right
                        )
                    )->simplify();
                }
            }
        }

        if(this->right->getType() == TypeNode::Power)
        {
            auto* right = dynamic_cast<Power<Type>*>(this->right.get());
            // x / x^a = x^(1 - a)
            if(this->left->equal(right->left))
            {
                return std::make_unique<Power<Type>>(
                    this->left,
                    std::make_unique<Subtraction<Type>>(
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0)),
                        right->right
                    )
                )->simplify();
            }
        }

        return std::make_unique<Division>(this->left, this->right);
    }
} // Math


#endif // DIVISION_HPP
