#include <cctype>
#include <utility>

#include "../include/Lexer.hpp"


namespace Math
{
    Lexer::Lexer(std::string expr)
        : source(std::move(expr))
        , pos {}
        , reserved {}
    {}


    Token Lexer::getNextToken()
    {
        if(!this->reserved.empty())
        {
            Token result {this->reserved.top()};
            this->reserved.pop();
            return result;
        }

        skipWhitespace();

        if(this->pos >= this->source.size())
        {
            return Token{TokenType::END, ""};
        }

        if(std::isalpha(this->source[this->pos]))
        {
            return getSymbolToken();
        }

        if(std::isdigit(this->source[this->pos]))
        {
            return getNumberToken();
        }

        Token result {TokenType::BAD_TOKEN, this->source.substr(this->pos++, 1)};

        switch(this->source[this->pos - 1])
        {
            case '+': result.type = TokenType::PLUS;  break;
            case '-': result.type = TokenType::MINUS; break;
            case '*': result.type = TokenType::STAR;  break;
            case '/': result.type = TokenType::SLASH; break;
            case '^': result.type = TokenType::POW;   break;
            case '(': result.type = TokenType::LPAR;  break;
            case ')': result.type = TokenType::RPAR;  break;
            case '=': result.type = TokenType::EQ;    break;
            default: ;
        }

        return result;
    }


    void Lexer::pushBack(const Token& token)
    {
        this->reserved.push(token);
    }


    Token Lexer::getSymbolToken()
    {
        const std::size_t start {this->pos};
        while(this->pos < this->source.size() &&
            (std::isalnum(this->source[this->pos]) || this->source[this->pos] == '_'))
        {
            this->pos++;
        }
        return Token{TokenType::SYMBOL, this->source.substr(start, this->pos - start)};
    }


    Token Lexer::getNumberToken()
    {
        std::size_t start{this->pos};
        while(this->pos < this->source.size() && std::isdigit(this->source[this->pos]))
        {
            this->pos++;
        }

        if(this->pos < this->source.size() && this->source[this->pos] == '.')
        {
            this->pos++;
            while(this->pos < this->source.size() && std::isdigit(this->source[this->pos]))
            {
                this->pos++;
            }
        }

        if(this->source[this->pos - 1] == '.')
        {
            return Token{TokenType::BAD_TOKEN,this->source.substr(start, this->pos - start)};
        }

        return Token{TokenType::NUMBER,this->source.substr(start, this->pos - start)};
    }


    void Lexer::skipWhitespace()
    {
        while(this->pos < this->source.size() && std::isspace(this->source[this->pos]))
        {
            this->pos++;
        }
    }
} // Math
