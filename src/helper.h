#ifndef HELPER_H
#define HELPER_H

#include <string>

namespace helper
{

void convertArithmeticExpFromFile(std::string fileName);
void convertLogicalExpFromFile(std::string fileName);
void evaluateArithmeticExpFromFile(std::string fileName);
void evaluateLogicalExpFromFile(std::string fileName);

};  // namespace helper

std::string Infix2Postfix(std::string infix);
std::string Infix2Prefix(std::string infix);
std::string PostfixPrefixCalculator(std::string input);

std::string LogicInfix2Postfix(std::string infix);
std::string LogicInfix2Prefix(std::string infix);
std::string LogicPostfixPrefixCalculator(std::string input, std::string varlue);

#endif
