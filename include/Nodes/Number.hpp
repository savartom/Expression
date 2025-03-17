#ifndef NUMBER_HPP
#define NUMBER_HPP


#include <sstream>
#include <complex>

#include "Node.hpp"


namespace Math
{
    template<typename Type>
    class Number final : public Node<Type>
    {
    public:
        Number(const Number& number);
        explicit Number(Type value);

        std::unique_ptr<Node<Type>> makeCopy() override;

        Priority getPriority() override;

        TypeNode getType() override;

        bool equal(std::unique_ptr<Node<Type>>& ptr) override;

        std::string toString() override;

        std::unique_ptr<Node<Type>> substitute(const std::string& variable, const std::unique_ptr<Node<Type>>& expression) override;

        Type calculate(const std::vector<std::string>& variable, const std::vector<Type>& value) override;

        std::unique_ptr<Node<Type>> differentiate(const std::string& variable) override;

        std::unique_ptr<Node<Type>> simplify() override;

        Type value;
    };
} // Math


namespace Math
{
    template<typename Type>
    Number<Type>::Number(const Number &number)
    {
        this->value = number.value;
    }


    template<typename Type>
    Number<Type>::Number(Type value)
        : value(value)
    {
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Number<Type>::makeCopy()
    {
        return std::make_unique<Number>(this->value);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Number<Type>::substitute(const std::string &variable, const std::unique_ptr<Node<Type>>& expression)
    {
        return std::make_unique<Number>(*this);
    }


    template<typename Type>
    Type Number<Type>::calculate(const std::vector<std::string>& variable, const std::vector<Type>& value)
    {
        return this->value;
    }


    template<typename Type>
    Priority Number<Type>::getPriority()
    {
        if(std::is_same_v<Type, std::complex<float>> ||
           std::is_same_v<Type, std::complex<double>> ||
           std::is_same_v<Type, std::complex<long double>>)
        {
            std::string real = to_string(std::real(this->value));
            std::string imag = to_string(std::imag(this->value));
            if((real != "0" && real != "-0") && (imag != "0" && imag != "-0"))
            {
                return Priority::Addition;
            }
            return Priority::Number;
        }
        return Priority::Number;
    }


    template<typename Type>
    TypeNode Number<Type>::getType()
    {
        return TypeNode::Number;
    }


    template<typename Type>
    bool Number<Type>::equal(std::unique_ptr<Node<Type>>& ptr)
    {
        if(this->getType() == ptr->getType())
        {
            auto* number = dynamic_cast<Number*>(ptr.get());
            return this->value == number->value;
        }
        return false;
    }


    template<typename Type>
    std::string Number<Type>::toString()
    {
        if(std::is_same_v<Type, std::complex<float>> ||
           std::is_same_v<Type, std::complex<double>> ||
           std::is_same_v<Type, std::complex<long double>>)
        {
            std::string real = to_string(std::real(this->value));
            std::string imag = to_string(std::imag(this->value));

            if(real == "-0")
            {
                real = "0";
            }
            if(imag == "-0")
            {
                imag = "0";
            }

            if(real == "0" && imag == "0")
            {
                return "0";
            }
            if(real == "0")
            {
                if(imag == "1")
                {
                    return "i";
                }
                if(imag == "-1")
                {
                    return "-i";
                }
                return imag + "i";
            }
            if(imag == "0")
            {
                return real;
            }

            if(imag.front() == '-')
            {
                imag = imag.substr(1);
                if(imag == "1")
                {
                    return real + " - i";
                }
                return real + " - " + imag + "i";
            }
            if(imag == "1")
            {
                return real + " + i";
            }
            return real + " + " + imag + "i";
        }
        return to_string(this->value);
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Number<Type>::differentiate(const std::string &variable)
    {
        return std::make_unique<Number<Type>>(Type{});
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Number<Type>::simplify()
    {
        return std::make_unique<Number>(*this);
    }
} // Math


#endif // NUMBER_HPP
