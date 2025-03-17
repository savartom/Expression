#ifndef MINUS_HPP
#define MINUS_HPP


#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Minus final : public Node<Type>
    {
    public:
        Minus(const Minus& minus);
        explicit Minus(const std::unique_ptr<Node<Type>>& argument);

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
    Minus<Type>::Minus(const Minus& minus)
    {
        this->argument = minus.argument->makeCopy();
    }


    template<typename Type>
    Minus<Type>::Minus(const std::unique_ptr<Node<Type>>& argument)
    {
        this->argument = argument->makeCopy();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Minus<Type>::makeCopy()
    {
        return std::make_unique<Minus>(this->argument);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Minus<Type>::substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Minus>(this->argument->substitute(variable, expression));
    }


    template<typename Type>
    Type Minus<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return -this->argument->calculate(variable, value);
    }


    template<typename Type>
    Priority Minus<Type>::getPriority()
    {
        return Priority::Minus;
    }


    template<typename Type>
    TypeNode Minus<Type>::getType()
    {
        return TypeNode::Minus;
    }


    template<typename Type>
    bool Minus<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* minus = dynamic_cast<Minus*>(ptr.get());
            return this->argument->equal(minus->argument);
        }
        return false;
    }


    template<typename Type>
    std::string Minus<Type>::toString()
    {
        std::string string = this->argument->toString();
        if(string.front() == '-' || this->argument->getPriority() <= this->getPriority())
        {
            return "-(" + string + ")";
        }
        return "-" + string;
    }


    template<typename Type>
    std::unique_ptr<Node<Type> > Minus<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Minus>(this->argument->differentiate(variable));
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Minus<Type>::simplify()
    {
        this->argument = this->argument->simplify();

        // -(a) = -a
        if(this->argument->getType() == TypeNode::Number)
        {
            return std::make_unique<Number<Type>>(this->calculate({}, {}));
        }
        // -(-a) = a
        if(this->argument->getType() == TypeNode::Minus)
        {
            auto* arg = dynamic_cast<Minus*>(this->argument.get());
            return arg->argument->makeCopy();
        }

        return std::make_unique<Minus>(this->argument);
    }
} // Math


#endif // MINUS_HPP
