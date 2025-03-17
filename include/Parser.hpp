#ifndef PARSER_HPP
#define PARSER_HPP


#include <complex>
#include <memory>

#include "Lexer.hpp"
#include "Nodes/Node.hpp"
#include "Nodes/Number.hpp"
#include "Nodes/Variable.hpp"
#include "Nodes/Minus.hpp"
#include "Nodes/Addition.hpp"
#include "Nodes/Subtraction.hpp"
#include "Nodes/Sin.hpp"
#include "Nodes/Cos.hpp"
#include "Nodes/Exp.hpp"
#include "Nodes/Ln.hpp"
#include "Nodes/Multiplication.hpp"
#include "Nodes/Division.hpp"
#include "Nodes/Power.hpp"


namespace Math
{
    template<typename Type>
    class Parser : private Lexer
    {
    public:
        explicit Parser(std::string expression);

        std::unique_ptr<Node<Type>> parseExpression();
        std::pair<std::string, std::unique_ptr<Node<Type>>> parseAssignment();

    private:
        std::unique_ptr<Node<Type>> expr();
        std::unique_ptr<Node<Type>> sum();
        std::unique_ptr<Node<Type>> unary();
        std::unique_ptr<Node<Type>> mul();
        std::unique_ptr<Node<Type>> pow();
        std::unique_ptr<Node<Type>> primary();
        std::unique_ptr<Node<Type>> funcCall();
        std::unique_ptr<Node<Type>> group();
    };
} // Math



namespace Math
{
    template<typename Type>
    Parser<Type>::Parser(std::string expression)
        : Lexer(expression)
    {}


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::parseExpression()
    {
        auto res {this->expr()};
        if(this->getNextToken().type != TokenType::END)
        {
            throw std::invalid_argument("Can not parse expression");
        }
        return res;
    }


    template<typename Type>
    std::pair<std::string, std::unique_ptr<Node<Type>>> Parser<Type>::parseAssignment()
    {
        auto variable {this->getNextToken()};
        auto equal {this->getNextToken()};

        if(equal.type != TokenType::EQ || variable.type != TokenType::SYMBOL
           || variable.str == "sin" || variable.str == "cos" || variable.str == "exp" || variable.str == "ln"
           || (variable.str == "i" && (std::is_same_v<Type, std::complex<float>> ||
                                       std::is_same_v<Type, std::complex<double>> ||
                                       std::is_same_v<Type, std::complex<long double>>)))
        {
            throw std::invalid_argument("Can not parse assignment");
        }

        auto expression {parseExpression()};
        return {variable.str, std::move(expression)};
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::expr()
    {
        return this->sum();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::sum()
    {
        auto left {this->unary()};
        auto token {this->getNextToken()};

        while(token.type == TokenType::PLUS || token.type == TokenType::MINUS)
        {
            if(token.type == TokenType::PLUS)
            {
                left = std::make_unique<Addition<Type>>(left, this->mul());
            }
            else
            {
                left = std::make_unique<Subtraction<Type>>(left, this->mul());
            }
            token = this->getNextToken();
        }
        this->pushBack(token);

        return left;
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::unary()
    {
        auto token {this->getNextToken()};

        if(token.type == TokenType::MINUS)
        {
            return std::make_unique<Minus<Type>>(mul());
        }
        this->pushBack(token);

        return mul();
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::mul()
    {
        auto left {this->pow()};

        while(true)
        {
            auto token {this->getNextToken()};

            if(token.type == TokenType::STAR)
            {
                left = std::make_unique<Multiplication<Type>>(left, this->pow());
            }
            else if(token.type == TokenType::SLASH)
            {
                left = std::make_unique<Division<Type>>(left, this->pow());
            }
            else if(token.type == TokenType::SYMBOL || token.type == TokenType::LPAR)
            {
                this->pushBack(token);
                left = std::make_unique<Multiplication<Type>>(left, this->pow());
            }
            else
            {
                this->pushBack(token);
                break;
            }
        }

        return left;
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::pow()
    {
        auto left {this->primary()};
        auto token {this->getNextToken()};

        if(token.type == TokenType::POW)
        {
            return std::make_unique<Power<Type>>(left, this->pow());
        }
        this->pushBack(token);

        return left;
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::primary()
    {
        auto token {this->getNextToken()};

        if(token.type == TokenType::LPAR)
        {
            this->pushBack(token);
            return this->group();
        }

        if(token.type == TokenType::NUMBER)
        {
            return std::make_unique<Number<Type>>(getNumber<Type>(std::stod(token.str)));
        }

        if(token.type == TokenType::SYMBOL)
        {
            if(token.str == "sin" || token.str == "cos" || token.str == "exp" || token.str == "ln")
            {
                this->pushBack(token);
                return this->funcCall();
            }
            if(token.str == "i" && (std::is_same_v<Type, std::complex<float>> ||
                                    std::is_same_v<Type, std::complex<double>> ||
                                    std::is_same_v<Type, std::complex<long double>>))
            {
                return std::make_unique<Number<Type>>(getNumber<Type>(0.0, 1.0));
            }
            return std::make_unique<Variable<Type>>(token.str);
        }

        throw std::invalid_argument("Can not parse primary expression");
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::funcCall()
    {
        auto name {this->getNextToken()};

        if(name.type == TokenType::SYMBOL)
        {
            if(name.str == "sin")
            {
                return std::make_unique<Sin<Type>>(this->group());
            }
            if(name.str == "cos")
            {
                return std::make_unique<Cos<Type>>(this->group());
            }
            if(name.str == "exp")
            {
                return std::make_unique<Exp<Type>>(this->group());
            }
            if(name.str == "ln")
            {
                return std::make_unique<Ln<Type>>(this->group());
            }
        }

        throw std::invalid_argument("Can not parse function call");
    }


    template<typename Type>
    std::unique_ptr<Node<Type>> Parser<Type>::group()
    {
        if(this->getNextToken().type != TokenType::LPAR)
        {
            throw std::invalid_argument("Can not parse group expression");
        }

        auto result {this->expr()};

        if(this->getNextToken().type != TokenType::RPAR)
        {
            throw std::invalid_argument("Can not parse group expression");
        }

        return result;
    }
} // Math


#endif // PARSER_HPP
