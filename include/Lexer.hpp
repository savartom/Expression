#ifndef LEXER_HPP
#define LEXER_HPP


#include <cstddef>
#include <stack>
#include <string>


#include "Token.hpp"


namespace Math
{
    class Lexer
    {
    public:
        explicit Lexer(std::string expr);

        Token getNextToken();

        void pushBack(const Token& token);

    private:
        std::string source;
        std::size_t pos;
        std::stack<Token> reserved;

        Token getSymbolToken();
        Token getNumberToken();

        void skipWhitespace();
    };
} // Math


#endif // LEXER_HPP
