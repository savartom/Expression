#ifndef MULTIPLICATION_HPP
#define MULTIPLICATION_HPP


#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Multiplication final : public Node<Type>
    {
    public:
        Multiplication(const Multiplication& multiplication);
        Multiplication(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right);

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
    Multiplication<Type>::Multiplication(const Multiplication& multiplication)
    {
        this->left = multiplication.left->makeCopy();
        this->right = multiplication.right->makeCopy();
    }


    template<typename Type>
    Multiplication<Type>::Multiplication(const std::unique_ptr<Node<Type>>& left, const std::unique_ptr<Node<Type>>& right)
    {
        this->left = left->makeCopy();
        this->right = right->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Multiplication<Type>::makeCopy()
    {
        return std::make_unique<Multiplication>(this->left, this->right);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Multiplication<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Multiplication>(this->left->substitute(variable, expression), this->right->substitute(variable, expression));
    }


    template<typename Type>
    Type Multiplication<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return this->left->calculate(variable, value) * this->right->calculate(variable, value);
    }


    template<typename Type>
    Priority Multiplication<Type>::getPriority()
    {
        return Priority::Multiplication;
    }


    template<typename Type>
    TypeNode Multiplication<Type>::getType()
    {
        return TypeNode::Multiplication;
    }


    template<typename Type>
    bool Multiplication<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* multiplication = dynamic_cast<Multiplication*>(ptr.get());
            return this->left->equal(multiplication->left) && this->right->equal(multiplication->right);
        }
        return false;
    }


    template<typename Type>
    std::string Multiplication<Type>::toString()
    {
        std::string left = this->left->toString();
        std::string right = this->right->toString();
        if(this->left->getPriority() < this->getPriority())
        {
            left = "(" + left + ")";
        }
        if(right.front() == '-' || this->right->getPriority() < this->getPriority())
        {
            right = "(" + right + ")";
        }
        return left + " * " + right;
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Multiplication<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Addition<Type>>(
            std::make_unique<Multiplication<Type>>(
                this->left->differentiate(variable),
                this->right
            ),
            std::make_unique<Multiplication<Type>>(
                this->left,
                this->right->differentiate(variable)
            )
        );
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Multiplication<Type>::simplify()
    {
        this->left = this->left->simplify();
        this->right = this->right->simplify();

        // 0 * x = x * 0 = 0
        if(this->left->toString() == "0" || this->left->toString() == "-0" ||
           this->right->toString() == "0" || this->right->toString() == "-0")
        {
            return std::make_unique<Number<Type>>(Type{});
        }

        // 1 * x = x
        if(this->left->toString() == "1")
        {
            return this->right->makeCopy();
        }
        // -1 * x = -x
        if(this->left->toString() == "-1")
        {
            return std::make_unique<Minus<Type>>(this->right);
        }
        // x * 1 = x
        if(this->right->toString() == "1")
        {
            return this->left->makeCopy();
        }
        // x * -1 = x
        if(this->right->toString() == "-1")
        {
            return std::make_unique<Minus<Type>>(this->left);
        }

        // a * b = ab
        if(this->left->getType() == TypeNode::Number && this->right->getType() == TypeNode::Number)
        {
            return std::make_unique<Number<Type>>(this->calculate({}, {}));
        }

        if(this->left->getType() == TypeNode::Number)
        {
            auto* left = dynamic_cast<Number<Type>*>(this->left.get());

            if(this->right->getType() == TypeNode::Multiplication)
            {
                auto* right = dynamic_cast<Multiplication*>(this->right.get());
                // a * (b * x) = (a * b) * x
                if(right->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Multiplication>(
                        std::make_unique<Multiplication>(
                            this->left,
                            right->left
                        ),
                        right->right
                    )->simplify();
                }
                // a * (x * b) = (a * b) * x
                if(right->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Multiplication>(
                        std::make_unique<Multiplication>(
                            this->left,
                            right->right
                        ),
                        right->left
                    )->simplify();
                }
            }

            if(this->right->getType() == TypeNode::Division)
            {
                auto* right = dynamic_cast<Division<Type>*>(this->right.get());
                // a * (b / x) = (a * b) / x
                if(right->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Division<Type>>(
                        std::make_unique<Multiplication>(
                            this->left,
                            right->left
                        ),
                        right->right
                    )->simplify();
                }
                // a * (x / b) = (a / b) * x
                if(right->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Multiplication>(
                        std::make_unique<Division<Type>>(
                            this->left,
                            right->right
                        ),
                        right->left
                    )->simplify();
                }
            }
        }

        if(this->right->getType() == TypeNode::Number)
        {
            auto* right = dynamic_cast<Number<Type>*>(this->right.get());

            if(this->left->getType() == TypeNode::Multiplication)
            {
                auto* left = dynamic_cast<Multiplication*>(this->left.get());
                // (a * x) * b = (a * b) * x
                if(left->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Multiplication>(
                        std::make_unique<Multiplication>(
                            left->left,
                            this->right
                        ),
                        left->right
                    )->simplify();
                }
                // (x * a) * b = (a * b) * x
                if(left->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Multiplication>(
                        std::make_unique<Multiplication>(
                            left->right,
                            this->right
                        ),
                        left->left
                    )->simplify();
                }
            }

            if(this->left->getType() == TypeNode::Division)
            {
                auto* left = dynamic_cast<Division<Type>*>(this->left.get());
                // (a / x) * b = (a * b) / x
                if(left->left->getType() == TypeNode::Number)
                {
                    return std::make_unique<Division<Type>>(
                        std::make_unique<Multiplication>(
                            left->left,
                            this->right
                        ),
                        left->right
                    )->simplify();
                }
                // (x / a) * b = (b / a) * x
                if(left->right->getType() == TypeNode::Number)
                {
                    return std::make_unique<Multiplication>(
                        std::make_unique<Division<Type>>(
                            this->right,
                            left->right
                        ),
                        left->left
                    )->simplify();
                }
            }
        }

        // x * x = x^2
        if(this->left->equal(this->right))
        {
            return std::make_unique<Power<Type>>(
                this->left,
                std::make_unique<Number<Type>>(getNumber<Type>(2.0))
            );
        }

        // -x * -y = x * y
        if(this->left->getType() == TypeNode::Minus && this->right->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            return std::make_unique<Multiplication>(
                left->argument,
                right->argument
            )->simplify();
        }
        // -x * y = -(x * y)
        if(this->left->getType() == TypeNode::Minus)
        {
            auto* left = dynamic_cast<Minus<Type>*>(this->left.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Multiplication>(
                    left->argument,
                    this->right
                )
            )->simplify();
        }
        // x * -y = -(x * y)
        if(this->right->getType() == TypeNode::Minus)
        {
            auto* right = dynamic_cast<Minus<Type>*>(this->right.get());
            return std::make_unique<Minus<Type>>(
                std::make_unique<Multiplication>(
                    this->left,
                    right->argument
                )
            )->simplify();
        }

        // a / x * b / y = (a * b) / (x * y)
        if(this->left->getType() == TypeNode::Division && this->right->getType() == TypeNode::Division)
        {
            auto* left = dynamic_cast<Division<Type>*>(this->left.get());
            auto* right = dynamic_cast<Division<Type>*>(this->right.get());
            return std::make_unique<Division<Type>>(
                std::make_unique<Multiplication<Type>>(
                    left->left,
                    right->left
                ),
                std::make_unique<Multiplication<Type>>(
                    left->right,
                    right->right
                )
            )->simplify();
        }

        // (a / b) * c = (a * c) / b
        if(this->left->getType() == TypeNode::Division)
        {
            auto* left = dynamic_cast<Division<Type>*>(this->left.get());
            return std::make_unique<Division<Type>>(
                std::make_unique<Multiplication>(
                    left->left,
                    this->right
                ),
                left->right
            )->simplify();
        }

        // a * (b / c) = (a * b) / c
        if(this->right->getType() == TypeNode::Division)
        {
            auto* right = dynamic_cast<Division<Type>*>(this->right.get());
            return std::make_unique<Division<Type>>(
                std::make_unique<Multiplication>(
                    this->left,
                    right->left
                ),
                right->right
            )->simplify();
        }

        if(this->right->getType() == TypeNode::Division)
        {
            auto* right = dynamic_cast<Division<Type>*>(this->right.get());
            // x * a / x = a
            if(this->left->equal(right->right))
            {
                return right->left->makeCopy();
            }
            if(this->left->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(this->left.get());
                // x^b * a / x = a * x^(b - 1)
                if(right->right->equal(pow1->left))
                {
                    return std::make_unique<Multiplication>(
                        right->left,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Subtraction<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                // x^b * a / x^c = a * x^(b - c)
                if(right->right->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(right->right.get());
                    if(pow1->left->equal(pow2->left))
                    {
                        return std::make_unique<Multiplication>(
                            right->left,
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

        if(this->left->getType() == TypeNode::Multiplication)
        {
            auto* left = dynamic_cast<Multiplication*>(this->left.get());
            if(left->right->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(left->right.get());
                // (a * x^b) * x = a * x^(b + 1)
                if(this->right->equal(pow1->left))
                {
                    return std::make_unique<Multiplication>(
                        left->left,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Addition<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                // (a * x^b) * x^c = a * x^(b + c)
                if(this->right->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->right.get());
                    if(pow2->left->equal(pow1->left))
                    {
                        return std::make_unique<Multiplication>(
                            left->left,
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Addition<Type>>(
                                    pow1->right,
                                    pow2->right
                                )
                            )
                        )->simplify();
                    }
                }
            }
            if(left->left->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(left->left.get());
                // (x^b * a) * x = a * x^(b + 1)
                if(this->right->equal(pow1->left))
                {
                    return std::make_unique<Multiplication>(
                        left->right,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Addition<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                // (x^b * a) * x^c = a * x^(b + c)
                if(this->right->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->right.get());
                    if(pow2->left->equal(pow1->left))
                    {
                        return std::make_unique<Multiplication>(
                            left->right,
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Addition<Type>>(
                                    pow1->right,
                                    pow2->right
                                )
                            )
                        )->simplify();
                    }
                }
            }
            // (a * x) * x = a * x^2
            if(this->right->equal(left->right))
            {
                return std::make_unique<Multiplication>(
                    left->left,
                    std::make_unique<Power<Type>>(
                        this->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(2.0))
                    )
                )->simplify();
            }
            // (x * a) * x = a * x^2
            if(this->right->equal(left->left))
            {
                return std::make_unique<Multiplication>(
                    left->right,
                    std::make_unique<Power<Type>>(
                        this->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(2.0))
                    )
                )->simplify();
            }
        }

        if(this->right->getType() == TypeNode::Multiplication)
        {
            auto* right = dynamic_cast<Multiplication*>(this->right.get());
            if(right->right->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(right->right.get());
                // x * (a * x^b) = a * x^(b + 1)
                if(this->left->equal(pow1->left))
                {
                    return std::make_unique<Multiplication>(
                        right->left,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Addition<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                // x^c * (a * x^b) = a * x^(b + c)
                if(this->left->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->left.get());
                    if(pow2->left->equal(pow1->left))
                    {
                        return std::make_unique<Multiplication>(
                            right->left,
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Addition<Type>>(
                                    pow1->right,
                                    pow2->right
                                )
                            )
                        )->simplify();
                    }
                }
            }
            if(right->left->getType() == TypeNode::Power)
            {
                auto* pow1 = dynamic_cast<Power<Type>*>(right->left.get());
                // x * (x^b * a) = a * x^(b + 1)
                if(this->left->equal(pow1->left))
                {
                    return std::make_unique<Multiplication>(
                        right->right,
                        std::make_unique<Power<Type>>(
                            pow1->left,
                            std::make_unique<Addition<Type>>(
                                pow1->right,
                                std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                            )
                        )
                    )->simplify();
                }
                // x^c * (x^b * a) = a * x^(b + c)
                if(this->left->getType() == TypeNode::Power)
                {
                    auto* pow2 = dynamic_cast<Power<Type>*>(this->left.get());
                    if(pow2->left->equal(pow1->left))
                    {
                        return std::make_unique<Multiplication>(
                            right->right,
                            std::make_unique<Power<Type>>(
                                pow1->left,
                                std::make_unique<Addition<Type>>(
                                    pow1->right,
                                    pow2->right
                                )
                            )
                        )->simplify();
                    }
                }
            }
            // x * (x * a) = a * x^2
            if(this->left->equal(right->left))
            {
                return std::make_unique<Multiplication>(
                    right->right,
                    std::make_unique<Power<Type>>(
                        this->left,
                        std::make_unique<Number<Type>>(getNumber<Type>(2.0))
                    )
                )->simplify();
            }
            // x * (a * x) = a * x^2
            if(this->left->equal(right->right))
            {
                return std::make_unique<Multiplication>(
                    right->left,
                    std::make_unique<Power<Type>>(
                        this->left,
                        std::make_unique<Number<Type>>(getNumber<Type>(2.0))
                    )
                )->simplify();
            }
        }

        if(this->left->getType() == TypeNode::Power)
        {
            auto* left = dynamic_cast<Power<Type>*>(this->left.get());
            // x^a * x = x^(a + 1)
            if(this->right->equal(left->left))
            {
                return std::make_unique<Power<Type>>(
                    this->right,
                    std::make_unique<Addition<Type>>(
                        left->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    )
                )->simplify();
            }
            // x^a * x^b = x^(a + b)
            if(this->right->getType() == TypeNode::Power)
            {
                auto* right = dynamic_cast<Power<Type>*>(this->right.get());
                if(left->left->equal(right->left))
                {
                    return std::make_unique<Power<Type>>(
                        left->left,
                        std::make_unique<Addition<Type>>(
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
            // x * x^a = x^(a + 1)
            if(this->left->equal(right->left))
            {
                return std::make_unique<Power<Type>>(
                    this->left,
                    std::make_unique<Addition<Type>>(
                        right->right,
                        std::make_unique<Number<Type>>(getNumber<Type>(1.0))
                    )
                )->simplify();
            }
        }

        return std::make_unique<Multiplication>(this->left, this->right);
    }
} // Math


#endif // MULTIPLICATION_HPP
