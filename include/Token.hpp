#ifndef TOKEN_HPP
#define TOKEN_HPP


#include <string>


namespace Math
{
    enum class TokenType
    {
        END,

        PLUS, MINUS, STAR, SLASH, POW,

        LPAR, RPAR,

        EQ,

        SYMBOL, NUMBER,

        BAD_TOKEN
    };

    struct Token
    {
        TokenType type {};
        std::string str {};
    };
} // Math


#endif // TOKEN_HPP
