/*
 * Задача о правильной скобочной последовательности
 * Дана последовательность скобочных пар [],(),{}.
 * Нужно написать алгоритм, проверяющий корректность последовательности.
 * func validBracketSequence(str string) bool
 * assert(validBracketSequence("{}") == true)
 * assert(validBracketSequence("{[") == false)
 * assert(validBracketSequence("[{}]") == true)
 * assert(validBracketSequence("][{}") == false)
 */

#include <iostream>
#include <stack>

bool checkPairBrackets(char left, char right) {
    if ((left == '[' && right == ']') ||
        (left == '(' && right == ')') ||
        (left == '{' && right == '}')) {
        return true;
    }
    return false;
}

bool validBracketSequence(const std::string& brackets) {
    std::stack<char> left_brackets;
    for (auto c: brackets) {
        if (c == '[' || c == '(' || c == '{') {
            left_brackets.push(c);
        } else if (c == ']' || c == ')' || c == '}') {
            if (!left_brackets.empty() && checkPairBrackets(left_brackets.top(), c)) {
                left_brackets.pop();
            } else {
                return false;
            }
        } else {
            continue;
        }
    }
    return true;
}

int main(int, char *[]) {
    std::string brackets;
    std::cin >> brackets;
    if (validBracketSequence(brackets)) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
    return 0;
}