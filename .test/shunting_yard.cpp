
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <deque>

int shunting_yard() {
    struct Operator {
        uint8_t precedence = 0;
        uint8_t arguments = 0;
    };

    std::unordered_map<char, Operator> ops;
    ops['/'] = { 3, 2 };
    ops['*'] = { 3, 2 };
    ops['+'] = { 1, 2 };
    ops['-'] = { 1, 2 };

    std::string exp = "(-1+2*-4-3)/7";

    struct Token {
        std::string name = "";

        enum class Type : uint8_t {
            Unknown,
            Literal_Numeric,
            Operator,
            LPAREN,
            RPAREN,
        } type = Type::Unknown;

        Operator op;
    };

    std::deque<Token> holding, output;
    Token prev = { "", Token::Type::Unknown, 0, 0 };

    for (const char c : exp) {
        if (std::isdigit(c)) {
            output.push_back({ std::string(1, c), Token::Type::Literal_Numeric });
            prev = output.back();
        } else if (c == '(') {
            holding.push_front({ std::string(1, c), Token::Type::LPAREN });
            prev = holding.front();
        } else if (c == ')') {
            while (!holding.empty() && holding.front().type != Token::Type::LPAREN) {
                output.push_back(holding.front());
                holding.pop_front();
            }

            if (!holding.empty()) {
                std::cout << "Unexpected Parenthesis\n";
                holding.pop_front();
            }

            prev = { std::string(1, c), Token::Type::RPAREN };
        } else if (ops.contains(c)) {
            Operator new_op = ops[c];
            if (c == '-' || c == '+') {
                if (prev.type != Token::Type::Literal_Numeric && prev.type != Token::Type::RPAREN) {
                    new_op.arguments = 1;
                    new_op.precedence = 100;
                }
            }

            while (!holding.empty() && holding.front().type != Token::Type::LPAREN) {
                if (holding.front().type == Token::Type::Operator) {
                    Operator& holding_stack_op = holding.front().op;
                    if (holding_stack_op.precedence >= new_op.precedence) {
                        output.push_back(holding.front());
                        holding.pop_front();
                    } else
                        break;
                }
            }

            holding.push_front({ std::string(1, c), Token::Type::Operator, new_op });
            prev = holding.front();
        } else {
            std::cout << "Bad Token: '" << c << "'\n";
            return 0;
        }
    }

    while (!holding.empty()) {
        output.push_back(holding.front());
        holding.pop_front();
    }

    std::cout << "Expression = " << exp << "\n";
    std::cout << "RPN        = ";
    for (Token& t : output) {
        std::cout << t.name;
    }
    std::cout << "\n";

    std::deque<double> solve;
    for (Token& inst : output) {
        switch (inst.type) {
            case Token::Type::Literal_Numeric: {
                solve.push_front(std::stod(inst.name));
            }
            break;

            case Token::Type::Operator: {
                std::vector<double> mem(inst.op.arguments);
                for (uint8_t a = 0; a < inst.op.arguments; ++a) {
                    if (solve.empty()) {
                        std::cout << "Empty Stack! Bad Expression\n";
                    } else {
                        mem[a] = solve[0];
                        solve.pop_front();
                    }
                }

                double result = 0.0;
                if (inst.op.arguments == 2) {
                    if (inst.name[0] == '/') {
                        std::cout << mem[1] << "/" << mem[0] << "\n";
                        result = mem[1] / mem[0];
                    } else if (inst.name[0] == '*') {
                        std::cout << mem[1] << "*" << mem[0] << "\n";
                        result = mem[1] * mem[0];
                    } else if (inst.name[0] == '+') {
                        std::cout << mem[1] << "+" << mem[0] << "\n";
                        result = mem[1] + mem[0];
                    } else if (inst.name[0] == '-') {
                        std::cout << mem[1] << "-" << mem[0] << "\n";
                        result = mem[1] - mem[0];
                    }
                }

                if (inst.op.arguments == 1) {
                    if (inst.name[0] == '+') {
                        std::cout << "+" << mem[0] << "\n";
                        result = +mem[0];
                    } else if (inst.name[0] == '-') {
                        std::cout << "-" << mem[0] << "\n";
                        result = -mem[0];
                    }
                }

                solve.push_front(result);
            }
            break;

            case Token::Type::Unknown:
            default:
                std::cout << "Unknown Token!";
                return 0;
        }
    }

    std::cout << "Result     = " << std::to_string(solve[0]) << "\n";
    return 0;
}
