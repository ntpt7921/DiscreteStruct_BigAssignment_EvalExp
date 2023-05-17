#include "helper.h"
#include <algorithm>
#include <bits/stdc++.h>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <list>
#include <ostream>
#include <queue>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace helper
{

static const std::string whitespace = " \t\n\v\f\r";
static const std::string paren = "()";

void convertArithmeticExpFromFile(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return;

    std::string exp;
    while (std::getline(file, exp))
    {
        std::cout << exp << '\n';
        std::cout << "Postfix: " << Infix2Postfix(exp) << '\n';
        std::cout << "Prefix: " << Infix2Prefix(exp) << '\n';
        std::cout << "~~~~~~~~" << std::endl;
    }
}

void convertLogicalExpFromFile(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return;

    std::string exp;
    while (std::getline(file, exp))
    {
        std::cout << exp << '\n';
        std::cout << "Postfix: " << LogicInfix2Postfix(exp) << '\n';
        std::cout << "Prefix: " << LogicInfix2Prefix(exp) << '\n';
        std::cout << "~~~~~~~~" << std::endl;
    }
}

void evaluateArithmeticExpFromFile(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return;

    std::string exp;
    while (std::getline(file, exp))
    {
        std::cout << exp << '\n';
        std::cout << "Result: " << PostfixPrefixCalculator(exp) << '\n';
        std::cout << "~~~~~~~~" << std::endl;
    }
}

void evaluateLogicalExpFromFile(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return;

    std::string exp;
    while (std::getline(file, exp))
    {
        std::cout << exp << '\n';
        // TODO: fix this
        std::cout << "Result: " << LogicPostfixPrefixCalculator(exp, "") << '\n';
        std::cout << "~~~~~~~~" << std::endl;
    }
}

namespace arithmetic
{

static const std::string operatorChar = "+-*/^";
static const std::string digit = "0123456789";

struct SyntaxError : public std::exception
{
    const char *what() const throw() { return "syntax error"; }
};

struct MultipleOutputError : public std::exception
{
    const char *what() const throw() { return "multiple-output error"; }
};

struct UndefinedError : public std::exception
{
    const char *what() const throw() { return "undefined error"; }
};

struct DivideByZeroError : public std::exception
{
    const char *what() const throw() { return "divided-by-0 error"; }
};

enum class TokenType
{
    UNDEFINED,
    EMPTY,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPER,
    NUMBER,
};

std::ostream &operator<<(std::ostream &out, TokenType tk)
{
    switch (tk)
    {
        case TokenType::UNDEFINED:
            out << "UNDEFINED";
            break;
        case TokenType::EMPTY:
            out << "EMPTY";
            break;
        case TokenType::OPEN_PAREN:
            out << "OPEN_PAREN";
            break;
        case TokenType::CLOSE_PAREN:
            out << "CLOSE_PAREN";
            break;
        case TokenType::OPER:
            out << "OPER";
            break;
        case TokenType::NUMBER:
            out << "NUMBER";
            break;
        default:
            break;
    }
    return out;
}

std::string extractStringWithToken(std::string &exp)
{
    auto start = exp.find_first_not_of(whitespace);
    auto getTypeFromChar = [](char c) -> int
    {
        if (c == '.' || digit.find(c) != std::string::npos)
            return 0;  // NUMBER
        else if (paren.find(c) != std::string::npos)
            return 1;  // paren
        else if (operatorChar.find(c) != std::string::npos)
            return 2;  // operator
        else
            return 3;  // space and other stuff
    };

    int currType = getTypeFromChar(exp[start]);
    auto end = start + 1;
    while (end < exp.size())
    {
        if (exp[end] == ')' || exp[end] == '(')  // break on each paren
            break;
        else if (operatorChar.find(exp[end]) != std::string::npos)  // break on each operator
            break;
        else if (getTypeFromChar(exp[end]) != currType)  // else break by differ type
            break;
        end++;
    }

    if (start == std::string::npos)
    {
        exp.clear();
        return "";
    }
    else if (end == exp.size())
    {
        auto result = exp.substr(start);
        exp.clear();
        return result;
    }
    else
    {
        auto result = exp.substr(start, end - start);
        exp.erase(0, end);
        return result;
    }
}

TokenType getTokenTypeFromString(const std::string &str)
{
    if (str.find_first_not_of(whitespace) == std::string::npos)
        return TokenType::EMPTY;

    if (str.find("(") != std::string::npos)
        return TokenType::OPEN_PAREN;

    if (str.find(")") != std::string::npos)
        return TokenType::CLOSE_PAREN;

    if (str.find_first_of(operatorChar) != std::string::npos)
        return TokenType::OPER;

    if (str.find_first_of(digit) != std::string::npos)
    {
        auto firstDigit = str.find_first_of(digit);
        auto firstDot = str.find(".");

        // digit must come before '.'
        if (firstDigit >= firstDot)
            throw SyntaxError();

        if (firstDot != std::string::npos)
        {
            // if there is more than one '.', then this is a floating point error
            if (str.find(".", firstDot) != std::string::npos)
                throw SyntaxError();
            // if there are no digits after the dot, this is floating point error
            if (str.find_first_of(digit, firstDot) == std::string::npos)
                throw SyntaxError();
            return TokenType::NUMBER;
        }
        else
        {
            return TokenType::NUMBER;
        }
    }

    throw UndefinedError();
}

enum class Operator
{
    UNDEFINED,
    ADD_1,
    ADD_2,
    SUB_1,
    SUB_2,
    MUL,
    DIV,
    POWER,
};

std::ostream &operator<<(std::ostream &out, Operator op)
{
    switch (op)
    {
        case Operator::UNDEFINED:
            out << "UNDEFINED";
            break;
        case Operator::ADD_1:
            out << "ADD_1";
            break;
        case Operator::ADD_2:
            out << "ADD_2";
            break;
        case Operator::SUB_1:
            out << "SUB_1";
            break;
        case Operator::SUB_2:
            out << "SUB_2";
            break;
        case Operator::MUL:
            out << "MUL";
            break;
        case Operator::DIV:
            out << "DIV";
            break;
        case Operator::POWER:
            out << "POWER";
            break;
        default:
            break;
    }
    return out;
}

Operator getOperatorFromString(const std::string &oper, bool expectBinary)
{
    if (oper == "^")
        return Operator::POWER;
    else if (oper == "*")
        return Operator::MUL;
    else if (oper == "/")
        return Operator::DIV;
    else if (oper == "+")
        return expectBinary ? Operator::ADD_2 : Operator::ADD_1;
    else if (oper == "-")
        return expectBinary ? Operator::SUB_2 : Operator::SUB_1;
    else
        throw UndefinedError();
}

int operatorPrecedence(Operator op)
{
    switch (op)
    {
        case Operator::ADD_1:
        case Operator::SUB_1:
            return 4;

        case Operator::POWER:
            return 3;

        case Operator::MUL:
        case Operator::DIV:
            return 2;

        case Operator::ADD_2:
        case Operator::SUB_2:
            return 1;

        default:
            return -1;
    }
}

using Token = std::tuple<TokenType, Operator, std::string>;

std::list<Token> InfixString2PostfixTokens(std::string infix)
{
    std::stack<Token> stack;
    std::list<Token> result;
    bool expectBinaryOper = false;
    bool expectNumber = false;
    bool expectNotNumber = false;

    while (!infix.empty())
    {
        std::string token;
        token = extractStringWithToken(infix);

        // NOTE: Debug print
        /* std::cerr << token << '\n'; */

        switch (getTokenTypeFromString(token))
        {
            case TokenType::UNDEFINED:
                throw UndefinedError();

            case TokenType::EMPTY:
                break;

            case TokenType::OPEN_PAREN:
                stack.push(Token(TokenType::OPEN_PAREN, Operator::UNDEFINED, token));
                break;

            case TokenType::CLOSE_PAREN:
                if (stack.empty())
                    throw SyntaxError();

                while (std::get<TokenType>(stack.top()) != TokenType::OPEN_PAREN)
                {
                    result.push_back(stack.top());
                    stack.pop();

                    if (stack.empty())
                        throw SyntaxError();
                }
                stack.pop();
                break;

            case TokenType::OPER:
            {
                Operator oper = getOperatorFromString(token, expectBinaryOper);

                // no consecutive operator
                if (expectNumber && oper != Operator::ADD_1 && oper != Operator::SUB_1)
                    throw UndefinedError();

                if (oper != Operator::ADD_1 && oper != Operator::SUB_1)
                {
                    expectNumber = true;
                    expectNotNumber = false;
                    expectBinaryOper = false;
                }

                auto currPrecedence = operatorPrecedence(oper);
                // if curr oper is unary
                if (currPrecedence == operatorPrecedence(Operator::ADD_1) && !stack.empty()
                    && operatorPrecedence(std::get<Operator>(stack.top())) == currPrecedence)
                {
                    stack.push(Token(TokenType::OPER, oper, token));
                }
                else
                {
                    while (!stack.empty()
                           && currPrecedence <= operatorPrecedence(std::get<Operator>(stack.top())))
                    {
                        result.push_back(stack.top());
                        stack.pop();
                    }
                    stack.push(Token(TokenType::OPER, oper, token));
                }

                break;
            }

            case TokenType::NUMBER:
                if (expectNotNumber)
                    throw SyntaxError();

                result.push_back(Token(TokenType::NUMBER, Operator::UNDEFINED, token));
                expectBinaryOper = true;
                expectNumber = false;
                expectNotNumber = true;
                break;

            default:
                throw UndefinedError();
        }
    }

    while (!stack.empty())
    {
        if (std::get<TokenType>(stack.top()) == TokenType::OPEN_PAREN)
            throw SyntaxError();

        result.push_back(stack.top());
        stack.pop();
    }

    return result;
}

std::string PostfixTokens2PrefixString(const std::list<Token> &postfixTokens)
{
    std::stack<std::string> result;

    for (const Token &token : postfixTokens)
    {
        /* // NOTE: Debug print */
        /* std::cerr << std::get<std::string>(token) << '\t' << std::get<TokenType>(token) << '\t' */
        /*           << std::get<Operator>(token) << '\n'; */

        switch (std::get<TokenType>(token))
        {

            case TokenType::NUMBER:
                result.push(std::get<std::string>(token));
                break;

            case TokenType::OPER:
            {
                switch (std::get<Operator>(token))
                {
                    // unary
                    case Operator::ADD_1:
                    case Operator::SUB_1:
                    {
                        std::string op = result.top();
                        result.pop();
                        result.push(std::get<std::string>(token) + " " + op);
                    }
                    break;

                    // binary
                    case Operator::ADD_2:
                    case Operator::SUB_2:
                    case Operator::MUL:
                    case Operator::DIV:
                    case Operator::POWER:
                    {
                        std::string op1 = result.top();
                        result.pop();
                        std::string op2 = result.top();
                        result.pop();
                        result.push(std::get<std::string>(token) + " " + op2 + " " + op1);
                    }
                    break;

                    default:
                        throw UndefinedError();
                }
            }
            break;

            default:
                throw UndefinedError();
        }
    }

    return result.top();
}

bool isPostfix(std::string &exp)
{
    while (!exp.empty())
    {
        std::string token = extractStringWithToken(exp);
        switch (getTokenTypeFromString(token))
        {
            case TokenType::EMPTY:
                continue;
                break;
            case TokenType::NUMBER:
                exp = token + exp;
                return true;
            case TokenType::OPER:
                exp = token + exp;
                return false;
            default:
                throw UndefinedError();
        }
    }

    throw UndefinedError();
}

bool isPrefix(std::string &exp)
{
    while (!exp.empty())
    {
        std::string token = extractStringWithToken(exp);
        switch (getTokenTypeFromString(token))
        {
            case TokenType::EMPTY:
                continue;
                break;
            case TokenType::NUMBER:
                exp = token + exp;
                return false;
            case TokenType::OPER:
                exp = token + exp;
                return true;
            default:
                throw UndefinedError();
        }
    }

    throw UndefinedError();
}

double evalulatePostfix(std::string postfix, bool reverse = false)
{
    std::stack<double> stack;

    while (!postfix.empty())
    {
        std::string token = extractStringWithToken(postfix);
        switch (getTokenTypeFromString(token))
        {
            case TokenType::EMPTY:
                continue;
                break;
            case TokenType::NUMBER:
                stack.push(std::stod(token));
                break;
            case TokenType::OPER:
            {
                double op2 = stack.top();
                stack.pop();
                double op1 = stack.top();
                stack.pop();

                switch (getOperatorFromString(token, true))
                {
                    case Operator::ADD_2:
                        stack.push(op1 + op2);
                        break;
                    case Operator::SUB_2:
                        if (!reverse)
                            stack.push(op1 - op2);
                        else
                            stack.push(op2 - op1);
                        break;
                    case Operator::MUL:
                        stack.push(op1 * op2);
                        break;
                    case Operator::DIV:
                        if (!reverse)
                        {
                            if (op2 == 0.0)
                                throw DivideByZeroError();
                            stack.push(op1 / op2);
                        }
                        else
                        {
                            if (op1 == 0.0)
                                throw DivideByZeroError();
                            stack.push(op2 / op1);
                        }
                        break;
                    case Operator::POWER:
                        if (op1 == 0.0 && op2 == 0.0)
                            throw UndefinedError();
                        if (!reverse)
                            stack.push(std::pow(op1, op2));
                        else
                            stack.push(std::pow(op2, op1));
                        break;
                    default:
                        throw UndefinedError();
                }
            }
            break;

            default:
                throw UndefinedError();
        }
    }

    return stack.top();
}

double evalulatePrefix(std::string prefix)
{
    std::list<std::string> tokens;
    while (!prefix.empty())
    {
        std::string token = extractStringWithToken(prefix);
        switch (getTokenTypeFromString(token))
        {
            case TokenType::EMPTY:
                continue;
            case TokenType::NUMBER:
            case TokenType::OPER:
                tokens.push_back(token);
                break;
            default:
                throw UndefinedError();
        }
    }

    std::string postfix;
    for (auto iter = tokens.rbegin(), end = tokens.rend(); iter != end; iter++)
        postfix += *iter + " ";

    return evalulatePostfix(postfix, true);  // eval with reverse operand
}

std::string convertToString(double val)
{
    constexpr int bufferSize = 64;
    char *buff = new char[bufferSize];
    std::string result;

    for (int i = 4; i >= 0; i--)
    {
        std::snprintf(buff, bufferSize, "%.*f", i, val);
        result = std::string(buff);
        if (result.back() != '0')
            break;
    }

    delete[] buff;
    return result;
}

};  // namespace arithmetic

namespace logical
{

struct SyntaxError : public std::exception
{
    const char *what() const throw() { return "syntax error"; }
};

struct MultipleOutputError : public std::exception
{
    const char *what() const throw() { return "multiple-output error"; }
};

struct UndefinedError : public std::exception
{
    const char *what() const throw() { return "undefined error"; }
};

enum class TokenType
{
    UNDEFINED,
    EMPTY,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPER,
    PROP,
};

std::ostream &operator<<(std::ostream &out, TokenType tk)
{
    switch (tk)
    {
        case TokenType::UNDEFINED:
            out << "UNDEFINED";
            break;
        case TokenType::EMPTY:
            out << "EMPTY";
            break;
        case TokenType::OPEN_PAREN:
            out << "OPEN_PAREN";
            break;
        case TokenType::CLOSE_PAREN:
            out << "CLOSE_PAREN";
            break;
        case TokenType::OPER:
            out << "OPER";
            break;
        case TokenType::PROP:
            out << "PROP";
            break;
        default:
            break;
    }
    return out;
}

enum class Operator
{
    UNDEFINED,
    AND,
    OR,
    NOT,
    IMPLY,
    EQUIV,
};

std::ostream &operator<<(std::ostream &out, Operator op)
{
    switch (op)
    {
        case Operator::UNDEFINED:
            out << "UNDEFINED";
            break;
        case Operator::AND:
            out << "AND";
            break;
        case Operator::OR:
            out << "OR";
            break;
        case Operator::NOT:
            out << "NOT";
            break;
        case Operator::IMPLY:
            out << "IMPLY";
            break;
        case Operator::EQUIV:
            out << "EQUIV";
            break;
        default:
            break;
    }
    return out;
}

static const std::string multiCharOperator = "<->";
static const std::string singleCharOperator = "&|~";

std::string extractStringWithToken(std::string &exp)
{
    auto start = exp.find_first_not_of(whitespace);
    auto getTypeFromChar = [](char c) -> int
    {
        if (std::isalpha(c))
            return 0;  // proposition
        else if (paren.find(c) != std::string::npos)
            return 1;  // paren
        else if (singleCharOperator.find(c) != std::string::npos)
            return 2;  // single-char operator
        else if (multiCharOperator.find(c) != std::string::npos)
            return 3;  // multi-char operator
        else
            return 4;  // space and other stuff
    };

    int currType = getTypeFromChar(exp[start]);
    auto end = start + 1;
    while (end < exp.size())
    {
        if (exp[end] == ')' || exp[end] == '(')  // break on each paren
            break;
        else if (singleCharOperator.find(exp[end])
                 != std::string::npos)  // break on single-char operator
            break;
        else if (getTypeFromChar(exp[end]) != currType)  // else break by differ type
            break;
        end++;
    }

    if (start == std::string::npos)
    {
        exp.clear();
        return "";
    }
    else if (end == exp.size())
    {
        auto result = exp.substr(start);
        exp.clear();
        return result;
    }
    else
    {
        auto result = exp.substr(start, end - start);
        exp.erase(0, end);
        return result;
    }
}

TokenType getTokenTypeFromString(const std::string &str)
{
    if (str.find_first_not_of(whitespace) == std::string::npos)
        return TokenType::EMPTY;

    if (str.find("(") != std::string::npos)
        return TokenType::OPEN_PAREN;

    if (str.find(")") != std::string::npos)
        return TokenType::CLOSE_PAREN;

    if (str.find_first_of(singleCharOperator) != std::string::npos)
        return TokenType::OPER;

    if (str.find_first_of(multiCharOperator) != std::string::npos)
    {
        if (str == "->")
            return TokenType::OPER;
        else if (str == "<->")
            return TokenType::OPER;
        else
            throw UndefinedError();
    }

    // if every char within str is alphabetical
    if (std::find_if_not(str.begin(), str.end(), [](char c) { return std::isalpha(c); })
        == str.end())
        return TokenType::PROP;

    throw UndefinedError();
}

Operator getOperatorFromString(const std::string &oper)
{
    if (oper == "&")
        return Operator::AND;
    else if (oper == "|")
        return Operator::OR;
    else if (oper == "~")
        return Operator::NOT;
    else if (oper == "->")
        return Operator::IMPLY;
    else if (oper == "<->")
        return Operator::EQUIV;
    else
        throw UndefinedError();
}

int operatorPrecedence(Operator op)
{
    switch (op)
    {
        case Operator::NOT:
            return 4;

        case Operator::AND:
        case Operator::OR:
            return 3;

        case Operator::IMPLY:
            return 2;

        case Operator::EQUIV:
            return 1;

        default:
            return -1;
    }
}

using Token = std::tuple<TokenType, Operator, std::string>;

std::list<Token> InfixString2PostfixTokens(std::string infix)
{
    std::stack<Token> stack;
    std::list<Token> result;
    bool expectProp = false;
    bool expectBinaryOper = false;

    while (!infix.empty())
    {
        std::string token;
        token = extractStringWithToken(infix);

        // NOTE: Debug print
        /* std::cerr << token << '\n'; */

        switch (getTokenTypeFromString(token))
        {
            case TokenType::UNDEFINED:
                throw UndefinedError();

            case TokenType::EMPTY:
                break;

            case TokenType::OPEN_PAREN:
                stack.push(Token(TokenType::OPEN_PAREN, Operator::UNDEFINED, token));
                break;

            case TokenType::CLOSE_PAREN:
                if (stack.empty())
                    throw SyntaxError();

                while (std::get<TokenType>(stack.top()) != TokenType::OPEN_PAREN)
                {
                    result.push_back(stack.top());
                    stack.pop();

                    if (stack.empty())
                        throw SyntaxError();
                }
                stack.pop();
                break;

            case TokenType::OPER:
            {
                Operator oper = getOperatorFromString(token);

                // no consecutive operator
                if (expectBinaryOper && oper == Operator::NOT)
                    throw UndefinedError();
                if (expectProp && oper != Operator::NOT)
                    throw UndefinedError();

                if (oper != Operator::NOT)
                {
                    expectProp = true;
                    expectBinaryOper = false;
                }

                auto currPrecedence = operatorPrecedence(oper);
                // if curr oper is unary
                if (oper == Operator::NOT && !stack.empty()
                    && std::get<Operator>(stack.top()) == Operator::NOT)
                {
                    stack.push(Token(TokenType::OPER, oper, token));
                }
                else
                {
                    while (!stack.empty()
                           && currPrecedence <= operatorPrecedence(std::get<Operator>(stack.top())))
                    {
                        result.push_back(stack.top());
                        stack.pop();
                    }
                    stack.push(Token(TokenType::OPER, oper, token));
                }
            }
            break;

            case TokenType::PROP:
                if (expectBinaryOper)
                    throw SyntaxError();

                result.push_back(Token(TokenType::PROP, Operator::UNDEFINED, token));
                expectProp = false;
                expectBinaryOper = true;
                break;

            default:
                throw UndefinedError();
        }
    }

    while (!stack.empty())
    {
        if (std::get<TokenType>(stack.top()) == TokenType::OPEN_PAREN)
            throw SyntaxError();

        result.push_back(stack.top());
        stack.pop();
    }

    return result;
}

std::string PostfixTokens2PrefixString(const std::list<Token> &postfixTokens)
{
    std::stack<std::string> result;

    for (const Token &token : postfixTokens)
    {
        /* // NOTE: Debug print */
        /* std::cerr << std::get<std::string>(token) << '\t' << std::get<TokenType>(token) << '\t' */
        /*           << std::get<Operator>(token) << '\n'; */

        switch (std::get<TokenType>(token))
        {

            case TokenType::PROP:
                result.push(std::get<std::string>(token));
                break;

            case TokenType::OPER:
            {
                switch (std::get<Operator>(token))
                {
                    // unary
                    case Operator::NOT:
                    {
                        std::string op = result.top();
                        result.pop();
                        result.push(std::get<std::string>(token) + op);
                    }
                    break;

                    // binary
                    case Operator::AND:
                    case Operator::OR:
                    case Operator::IMPLY:
                    case Operator::EQUIV:
                    {
                        std::string op1 = result.top();
                        result.pop();
                        std::string op2 = result.top();
                        result.pop();
                        result.push(std::get<std::string>(token) + op2 + op1);
                    }
                    break;

                    default:
                        throw UndefinedError();
                }
            }
            break;

            default:
                throw UndefinedError();
        }
    }

    return result.top();
}

using PropValue = std::unordered_map<std::string, bool>;

PropValue readPropValue(std::string varlue)
{
    PropValue result;
    std::queue<std::string> propName;

    // extract prop name
    while (!varlue.empty())
    {
        std::string token = extractStringWithToken(varlue);
        if (std::find_if_not(varlue.begin(), varlue.end(), [](char c) { return std::isalpha(c); })
            == varlue.end())
            propName.push(token);
        else
            break;
    }

    // extract prop value
    while (!varlue.empty())
    {
        // use extract function from helper::arithmetic to extract number
        std::string token = helper::arithmetic::extractStringWithToken(varlue);
        if (std::find_if_not(varlue.begin(), varlue.end(), [](char c) { return std::isdigit(c); })
            == varlue.end())
        {
            result[propName.front()] = static_cast<bool>(std::stoi(token));
            propName.pop();
        }
        else
            throw;
    }

    if (!propName.empty())
        throw;

    return result;
}

};  // namespace logical

};  // namespace helper

std::string Infix2Postfix(std::string infix)
{
    using helper::arithmetic::MultipleOutputError;
    using helper::arithmetic::SyntaxError;
    using helper::arithmetic::Token;
    using helper::arithmetic::UndefinedError;

    std::string result;
    try
    {
        std::list<Token> tokens = helper::arithmetic::InfixString2PostfixTokens(infix);
        for (const Token &token : tokens)
            result += std::get<std::string>(token) + " ";
    }
    catch (const MultipleOutputError &err)
    {
        result = err.what();
    }
    catch (const SyntaxError &err)
    {
        result = err.what();
    }
    catch (const UndefinedError &err)
    {
        result = err.what();
    }

    return result;
}

std::string Infix2Prefix(std::string infix)
{
    using helper::arithmetic::MultipleOutputError;
    using helper::arithmetic::SyntaxError;
    using helper::arithmetic::Token;
    using helper::arithmetic::UndefinedError;

    std::string result;
    try
    {
        result = helper::arithmetic::PostfixTokens2PrefixString(
            helper::arithmetic::InfixString2PostfixTokens(infix));
    }
    catch (const MultipleOutputError &err)
    {
        result = err.what();
    }
    catch (const SyntaxError &err)
    {
        result = err.what();
    }
    catch (const UndefinedError &err)
    {
        result = err.what();
    }

    return result;
}

std::string PostfixPrefixCalculator(std::string input)
{
    using helper::arithmetic::DivideByZeroError;
    using helper::arithmetic::UndefinedError;

    std::string result;
    try
    {
        double res;
        if (helper::arithmetic::isPostfix(input))
            res = helper::arithmetic::evalulatePostfix(input);
        else if (helper::arithmetic::isPrefix(input))
            res = helper::arithmetic::evalulatePrefix(input);
        else
            throw UndefinedError();

        return helper::arithmetic::convertToString(res);
    }
    catch (const DivideByZeroError &err)
    {
        result = err.what();
    }
    catch (const UndefinedError &err)
    {
        result = err.what();
    }

    return result;
}

std::string LogicInfix2Postfix(std::string infix)
{
    using helper::logical::MultipleOutputError;
    using helper::logical::SyntaxError;
    using helper::logical::Token;
    using helper::logical::UndefinedError;

    std::string result;
    try
    {
        std::list<Token> tokens = helper::logical::InfixString2PostfixTokens(infix);
        for (const Token &token : tokens)
            result += std::get<std::string>(token);
    }
    catch (const MultipleOutputError &err)
    {
        result = err.what();
    }
    catch (const SyntaxError &err)
    {
        result = err.what();
    }
    catch (const UndefinedError &err)
    {
        result = err.what();
    }

    return result;
}

std::string LogicInfix2Prefix(std::string infix)
{
    using helper::logical::MultipleOutputError;
    using helper::logical::SyntaxError;
    using helper::logical::Token;
    using helper::logical::UndefinedError;

    std::string result;
    try
    {
        result = helper::logical::PostfixTokens2PrefixString(
            helper::logical::InfixString2PostfixTokens(infix));
    }
    catch (const MultipleOutputError &err)
    {
        result = err.what();
    }
    catch (const SyntaxError &err)
    {
        result = err.what();
    }
    catch (const UndefinedError &err)
    {
        result = err.what();
    }

    return result;
}

std::string LogicPostfixPrefixCalculator(std::string input, std::string varlue)
{
    (void) input;
    (void) varlue;
    return "";
    return "";
}
