#include <iostream>
#include <complex>
#include <chrono>
#include <map>

#include "include/Expression.hpp"
#include "include/Parser.hpp"


void help()
{
    std::cout << "Usage:\n"
                 "\n"
                 "Symbolic simplification:       ./differentiator --simplify <expression>\n"
                 "Example:                       ./differentiator --simplify \"sin(x)^2 + cos(x)^2\"\n"
                 "\n"
                 "Expression substitution:       ./differentiator --subs <expression> [<substitutions>]\n"
                 "Example:                       ./differentiator --subs \"exp(x * y)\" x=\"x * ln(y)\" y=1\n"
                 "\n"
                 "Evaluate an expression:        ./differentiator --eval <expression> [<substitutions>]\n"
                 "Example:                       ./differentiator --eval \"exp(x) + y^2\" x=1 y=5\n"
                 "\n"
                 "Calculate the nth derivative:  ./differentiator --diff <expression> --by <variable> --nth <number>\n"
                 "Example:                       ./differentiator --diff \"sin(x) / cos(x)\" --by x --nth 2\n";

    exit(EXIT_FAILURE);
}


void simplify(const std::string& string)
{
    Math::Expression<std::complex<double>> expression;
    try
    {
        expression = Math::Expression<std::complex<double>>(string);
    }
    catch(...)
    {
        std::cerr << "Failed to parse \"" << string << "\"\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Operation: simplify\n"
                 "Expression:\n"
                 "\t" << expression << "\n";

    expression = expression.simplify();

    std::cout << "-----------------------------------------------\n";
    std::cout << "After simplification:\n"
                 "\t" << expression << "\n";
}


void substitute(const std::string& string, const std::vector<std::string>& subs)
{
    Math::Expression<std::complex<double>> expression;
    try
    {
        expression = Math::Expression<std::complex<double>>(string);
    }
    catch(...)
    {
        std::cerr << "Failed to parse \"" << string << "\"\n";
        exit(EXIT_FAILURE);
    }

    std::string incorrect {};
    std::map<std::string, Math::Expression<std::complex<double>>> substitutions;
    try
    {
        for(const auto& assignment : subs)
        {
            incorrect = assignment;
            auto [variable, exp] = Math::Parser<std::complex<double>>(assignment).parseAssignment();
            if(substitutions.contains(variable))
            {
                std::cerr << "Double assignment of the variable \"" << variable << "\"\n";
                exit(EXIT_FAILURE);
            }
            substitutions[variable] = Math::Expression(exp);
        }
    }
    catch(...)
    {
        std::cerr << "Failed to parse \"" << incorrect << "\"\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Operation: substitute\n"
                 "Expression:\n"
                 "\t" << expression << "\n";
    std::cout << "Substitutions:\n";
    for(const auto& [variable, value] : substitutions)
    {
        std::cout << "\t" << variable << "=" << value << "\n";
        expression = expression.substitute(variable, value);
    }

    std::cout << "-----------------------------------------------\n";
    std::cout << "After substitution and simplification:\n"
                 "\t" << expression << "\n";
}


void evaluate(const std::string& string, const std::vector<std::string>& subs)
{
    Math::Expression<std::complex<double>> expression;
    try
    {
        expression = Math::Expression<std::complex<double>>(string);
    }
    catch(...)
    {
        std::cerr << "Failed to parse \"" << string << "\"\n";
        exit(EXIT_FAILURE);
    }

    std::string incorrect {};
    std::map<std::string, Math::Expression<std::complex<double>>> names;
    std::vector<std::string> variables;
    std::vector<std::complex<double>> values;
    try
    {
        for(const auto& assignment : subs)
        {
            incorrect = assignment;
            auto [variable, exp] = Math::Parser<std::complex<double>>(assignment).parseAssignment();
            if(names.contains(variable))
            {
                std::cerr << "Double assignment of the variable \"" << variable << "\"\n";
                exit(EXIT_FAILURE);
            }
            names[variable] = Math::Expression(exp);
            variables.emplace_back(variable);
            values.emplace_back(exp->calculate({}, {}));
        }
    }
    catch(...)
    {
        std::cerr << "Failed to parse \"" << incorrect << "\"\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Operation: evaluate\n"
                 "Expression:\n"
                 "\t" << expression << "\n";
    std::cout << "Substitutions:\n";
    for(const auto& [variable, value] : names)
    {
        std::cout << "\t" << variable << "=" << value << "\n";
    }

    Math::Expression result {expression.calculate(variables, values)};

    std::cout << "-----------------------------------------------\n";
    std::cout << "Calculated value:\n"
                 "\t" << result << "\n";
}


void differentiate(const std::string& string, const std::string& variable, int number)
{
    Math::Expression<std::complex<double>> expression;
    try
    {
        expression = Math::Expression<std::complex<double>>(string);
    }
    catch(...)
    {
        std::cerr << "Failed to parse \"" << string << "\"\n";
        exit(EXIT_FAILURE);
    }

    if(Math::Parser<std::complex<double>>(variable).parseExpression()->getType() != Math::TypeNode::Variable)
    {
        std::cerr << "Incorrect variable \"" << variable << "\"\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Operation: differentiate\n"
                 "With respect to:\n"
                 "\t" << variable << "\n"
                 "Derivative number:\n"
                 "\t" << number << "\n"
                 "Expression:\n"
                 "\t" << expression << "\n";

    auto start{ std::chrono::high_resolution_clock::now() };

    expression = expression.differentiate(variable, number);

    auto end{ std::chrono::high_resolution_clock::now() };
    auto duration { std::chrono::duration_cast<std::chrono::milliseconds>(end - start) };

    std::cout << "-----------------------------------------------\n";
    std::cout << "After differentiation and simplification:\n"
                 "\t" << expression << "\n";
    std::cout << "Calculation time: " << duration << '\n';
}


int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        help();
    }

    if(argv[1] == std::string("--simplify"))
    {
        if(argc == 3)
        {
            simplify(argv[2]);
        }
        else
        {
            std::cerr << "Incorrect number of arguments\n";
            return EXIT_FAILURE;
        }
    }
    else if(argv[1] == std::string("--subs"))
    {
        std::vector<std::string> substitutions;
        for(int i = 3; i < argc; i++)
        {
            substitutions.emplace_back(argv[i]);
        }
        substitute(argv[2], substitutions);
    }
    else if(argv[1] == std::string("--eval"))
    {
        std::vector<std::string> substitutions;
        for(int i = 3; i < argc; i++)
        {
            substitutions.emplace_back(argv[i]);
        }
        evaluate(argv[2], substitutions);
    }
    else if(argv[1] == std::string("--diff"))
    {
        int number {1};
        std::string variable {"x"};
        int flag_by {-1};
        int flag_nth {-1};

        if(argc == 5)
        {
            if(argv[3] == std::string("--by"))
            {
                flag_by = 4;
            }
            else if(argv[3] == std::string("--nth"))
            {
                flag_nth = 4;
            }
            else
            {
                std::cerr << "Incorrect arguments\n";
                return EXIT_FAILURE;
            }
        }
        else if(argc == 7)
        {
            if(argv[3] == std::string("--by") && argv[5] == std::string("--nth"))
            {
                flag_by = 4;
                flag_nth = 6;
            }
            else if(argv[5] == std::string("--by") && argv[3] == std::string("--nth"))
            {
                flag_by = 6;
                flag_nth = 4;
            }
            else
            {
                std::cerr << "Incorrect arguments\n";
                return EXIT_FAILURE;
            }
        }
        else if(argc != 3)
        {
            std::cerr << "Incorrect number of arguments\n";
            return EXIT_FAILURE;
        }

        if(flag_by != -1)
        {
            variable = argv[flag_by];
        }
        if(flag_nth != -1)
        {
            try
            {
                number = std::stoi(argv[flag_nth]);
            }
            catch(...)
            {
                std::cerr << "Conversion error\n";
                return EXIT_FAILURE;
            }
            if(number <= 0)
            {
                std::cerr << "The number of the derivative must be positive\n";
                return EXIT_FAILURE;
            }
        }

        differentiate(argv[2], variable, number);
    }
    else
    {
        help();
    }

    return 0;
}
