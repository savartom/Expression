#ifndef VARIABLE_HPP
#define VARIABLE_HPP


#include <algorithm>

#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Variable final : public Node<Type>
    {
    public:
        Variable(const Variable& variable);
        explicit Variable(const std::string& name);

        std::unique_ptr<Node<Type>> makeCopy() override;

        Priority getPriority() override;

        TypeNode getType() override;

        bool equal(std::unique_ptr<Node<Type>>& ptr) override;

        std::string toString() override;

        std::unique_ptr<Node<Type>> substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression) override;

        Type calculate(const std::vector<std::string>& variable, const std::vector<Type>& value) override;

        std::unique_ptr<Node<Type>> differentiate(const std::string& variable) override;

        std::unique_ptr<Node<Type>> simplify() override;

        std::string name;
    };
} // Math


namespace Math
{
    template<typename Type>
    Variable<Type>::Variable(const Variable &variable)
    {
        this->name = variable.name;
    }


    template<typename Type>
    Variable<Type>::Variable(const std::string& name)
        : name(name)
    {
        if(name.empty())
        {
            throw std::invalid_argument("Empty variable");
        }
        if(!std::isalpha(name.front()))
        {
            throw std::invalid_argument("The variable must start with a letter");
        }
        for(const char c : name)
        {
            if(!(std::isalpha(c) || std::isdigit(c)))
            {
                throw std::invalid_argument("The variable can only consist of letters and digits");
            }
        }
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Variable<Type>::makeCopy()
    {
        return std::make_unique<Variable>(this->name);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Variable<Type>::substitute(const std::string &variable, const std::unique_ptr<Node<Type>>& expression)
    {
        if(this->name == variable)
        {
            return expression->makeCopy();
        }
        return this->makeCopy();
    }


    template<typename Type>
    Type Variable<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        auto iter = std::find(variable.begin(), variable.end(), this->name);
        if(iter == variable.end())
        {
            throw std::invalid_argument("The variable \"" + this->name + "\" has no value");
        }
        auto index = iter - variable.begin();
        return *(value.begin() + index);
    }


    template<typename Type>
    Priority Variable<Type>::getPriority()
    {
        return Priority::Variable;
    }


    template<typename Type>
    TypeNode Variable<Type>::getType()
    {
        return TypeNode::Variable;
    }


    template<typename Type>
    bool Variable<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* variable = dynamic_cast<Variable*>(ptr.get());
            return this->name == variable->name;
        }
        return false;
    }


    template<typename Type>
    std::string Variable<Type>::toString()
    {
        return this->name;
    }


    template<typename Type>
    std::unique_ptr<Node<Type> > Variable<Type>::differentiate(const std::string &variable)
    {
        if(this->name == variable)
        {
            return std::make_unique<Number<Type>>(getNumber<Type>(1.0));
        }
        return std::make_unique<Number<Type>>(Type{});
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Variable<Type>::simplify()
    {
        return std::make_unique<Variable>(*this);
    }
} // Math


#endif // VARIABLE_HPP
