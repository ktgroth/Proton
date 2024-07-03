
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <deque>

static constexpr auto make_LUT(const std::string_view accepted_chars) {
    std::array<bool, 256> lut { 0 };
    for (const auto c : accepted_chars)
        lut.at(uint8_t(c)) = true;

    return lut;
}

constexpr auto whitespace_digits = make_LUT(" \t\n\r\v\f\0");
constexpr auto numeric_digits = make_LUT("0123456789");
constexpr auto real_numeric_digits = make_LUT("0123456789.");
constexpr auto hex_numeric_digits = make_LUT("0123456789abcdefABCDEF");
constexpr auto bin_numeric_digits = make_LUT("01");
constexpr auto operator_digits = make_LUT("!$%^&*+-=#@?|`/\\<>~");
constexpr auto symbol_digits = make_LUT("abcdefghijklmnopqrstuvwxyz.ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789");


struct token {
    enum class Type : uint8_t {
        LSTRING,
        LNUM,
        OP,
        SYMBOL,
        KEYWORD,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        SEMI,
        COMMA,
        Unknown,
    } type = Type::Unknown;

    std::string symbol = "";
    double value = 0;
    
    std::string str() const {
        std::string o;
        switch (type) {
            case token::Type::LSTRING:
                o += "[LSTRING]";
                break;

            case token::Type::LNUM:
                o += "[LNUM   ]";
                break;

            case token::Type::OP:
                o += "[OP     ]";
                break;

            case token::Type::SYMBOL:
                o += "[SYMBOL ]";
                break;

            case token::Type::KEYWORD:
                o += "[KEYWORD]";

            case token::Type::LPAREN:
                o += "[LPAREN ]";
                break;

            case token::Type::RPAREN:
                o += "[RPAREN ]";
                break;

            case token::Type::LBRACE:
                o += "[LBRACE ]";
                break;

            case token::Type::RBRACE:
                o += "[RBRACE ]";
                break;

            case token::Type::SEMI:
                o += "[SEMI   ]";
                break;

            case token::Type::COMMA:
                o += "[COMMA  ]";
                break;

            case token::Type::Unknown:
                o += "[Unknown]";
                break;
        }

        o += " : " + symbol;
        if (type == token::Type::LNUM)
            o += " [" + std::to_string(value) + "]";

        return o;
    }
};

struct op {
    uint8_t precedence = 0;
    uint8_t arguments = 0;
};

struct keyword {
    uint8_t placeholder = 0;
};

class CompileError : public std::exception {
    std::string message;
        
public:
    CompileError(const std::string& msg) {
        message = msg;
    }

    const char *what() const noexcept override {
        return message.c_str();
    }
};

class Compiler {
protected:
    std::unordered_map<std::string, op> ops;
    std::unordered_map<std::string, keyword> keywords;

public:
    Compiler() {
        ops["*"] = { 3, 2 };
        ops["/"] = { 3, 2 };
        ops["+"] = { 1, 2 };
        ops["-"] = { 1, 2 };

        ops["u-"] = { 100, 1 };
        ops["u+"] = { 100, 1 };
    }

    std::vector<token> parse(const std::string& input) {
        if (input.empty())
            throw CompileError("[PARSER] No input provided");

        std::vector<token> output_tokens;
        enum class TokeniserState : uint8_t {
            NEW = 0,
            LSTRING = 1,
            LNUM = 2,
            LFANCY = 3,
            LHEX = 4,
            LBIN = 5,
            OP = 6,
            SYMBOL = 7,
            LPAREN = 8,
            RPAREN = 9,
            LBRACE = 10,
            RBRACE = 11,
            SEMI = 12,
            COMMA = 13,
            COMPLETE = 14,
        };

        TokeniserState stateNow = TokeniserState::NEW;
        TokeniserState stateNext = TokeniserState::NEW;
        
        std::string current_token = "";
        std::string fancy_numeric_token = "";
        token tok_current;
        
        bool decimal_point_found = false;
        size_t paren_balance = 0;
        size_t brace_balance = 0;

        auto charNow = input.begin();
        while (charNow <= input.end()) {
            switch (stateNow) {
                case TokeniserState::NEW: {
                    current_token.clear();
                    tok_current = { token::Type::Unknown, "" };
                    decimal_point_found = false;

                    if (whitespace_digits.at(*charNow)) {
                        stateNext = TokeniserState::NEW;
                        ++charNow;
                    } else if (real_numeric_digits.at(*charNow)) {
                        current_token = *charNow;
                        if (*charNow == '0') {
                            stateNext = TokeniserState::LFANCY;
                        } else {
                            stateNext = TokeniserState::LNUM;
                        }
                        ++charNow;
                    } else if (operator_digits.at(*charNow)) {
                        stateNext = TokeniserState::OP;
                    } else if (*charNow == '(') {
                        stateNext = TokeniserState::LPAREN;
                    } else if (*charNow == ')') {
                        stateNext = TokeniserState::RPAREN;
                    } else if (*charNow == '{') {
                        stateNext = TokeniserState::LBRACE;
                    } else if (*charNow == '}') {
                        stateNext = TokeniserState::RBRACE;
                    } else if (*charNow == ';') {
                        stateNext = TokeniserState::SEMI;
                    } else if (*charNow == ',') {
                        stateNext = TokeniserState::COMMA;
                    } else if (*charNow == '\"') {
                        stateNext = TokeniserState::LSTRING;
                        ++charNow;
                    } else {
                        current_token = *charNow;
                        if (*charNow != '\0')
                            stateNext = TokeniserState::SYMBOL;
                        ++charNow;
                    }
                }
                break;

                case TokeniserState::LSTRING: {
                    if (*charNow == '\"') {
                        stateNext = TokeniserState::COMPLETE;
                        tok_current = { token::Type::LSTRING, current_token };
                        ++charNow;
                    } else {
                        current_token += *charNow;
                        ++charNow;
                    }
                }
                break;

                case TokeniserState::LNUM: {
                    if (real_numeric_digits.at(*charNow)) {
                        if (*charNow == '.') {
                            if (decimal_point_found) {
                                throw CompileError("[PARSER] Bad numeric construction");
                            } else {
                                decimal_point_found = true;
                            }
                        }

                        current_token += *charNow;
                        stateNext = TokeniserState::LNUM;
                        ++charNow;
                    } else {
                        if (symbol_digits.at(*charNow))
                            throw CompileError("[PARSER] Invalid number/symbol");
                        
                        stateNext = TokeniserState::COMPLETE;
                        tok_current = { token::Type::LNUM, current_token };

                        tok_current.value = std::stod(current_token);
                    }
                }
                break;

                case TokeniserState::LFANCY: {
                    if (*charNow == 'x') {
                        current_token += *charNow;
                        fancy_numeric_token.clear();
                        stateNext = TokeniserState::LHEX;
                        ++charNow;
                    } else if (*charNow == 'b') {
                        current_token += *charNow;
                        fancy_numeric_token.clear();
                        stateNext = TokeniserState::LBIN;
                        ++charNow;
                    } else if (real_numeric_digits.at(*charNow)) {
                        stateNext = TokeniserState::LNUM;
                    } else
                        throw CompileError("[PARSER] Bad numeric literal");
                }
                break;

                case TokeniserState::LHEX: {
                    if (hex_numeric_digits.at(*charNow)) {
                        current_token += *charNow;
                        fancy_numeric_token += *charNow;
                        stateNext = TokeniserState::LHEX;
                        ++charNow;
                    } else {
                        if (symbol_digits.at(*charNow))
                            throw CompileError("[PARSER] Hexadecimal literal is malformed");

                        stateNext = TokeniserState::COMPLETE;
                        tok_current = { token::Type::LNUM, current_token };

                        tok_current.value = (double) std::stoll(fancy_numeric_token, nullptr, 16);
                    }
                }
                break;

                case TokeniserState::LBIN: {
                    if (bin_numeric_digits.at(*charNow)) {
                        current_token += *charNow;
                        fancy_numeric_token += *charNow;
                        stateNext = TokeniserState::LBIN;
                        ++charNow;
                    } else {
                        if (symbol_digits.at(*charNow))
                            throw CompileError("[PARSER] Binary literal is malformed");

                        stateNext = TokeniserState::COMPLETE;
                        tok_current = { token::Type::LNUM, current_token };

                        tok_current.value = (double) std::stoll(fancy_numeric_token, nullptr, 2);
                    }
                }
                break;

                case TokeniserState::OP: {
                    if (operator_digits.at(*charNow)) {
                        if (ops.contains(current_token + *charNow)) {
                            current_token += *charNow;
                            ++charNow;
                        } else {
                            if (ops.contains(current_token)) {
                                tok_current = { token::Type::OP, current_token };
                                stateNext = TokeniserState::COMPLETE;
                            } else {
                                current_token += *charNow;
                                ++charNow;
                            }
                        }
                    } else {
                        if (ops.contains(current_token)) {
                            tok_current = { token::Type::OP, current_token };
                            stateNext = TokeniserState::COMPLETE;
                        } else {
                            throw CompileError("[PARSER] Unrecognised operator: " + current_token);
                        }
                    }
                }
                break;

                case TokeniserState::SYMBOL: {
                    if (symbol_digits.at(*charNow)) {
                        current_token += *charNow;
                        ++charNow;
                    } else {
                        if (keywords.contains(current_token))
                            tok_current = { token::Type::KEYWORD, current_token };
                        else
                            tok_current = { token::Type::SYMBOL, current_token };
                        stateNext = TokeniserState::COMPLETE;
                    }
                }
                break;

                case TokeniserState::LPAREN: {
                    current_token += *charNow;
                    ++charNow;

                    ++paren_balance;
                    tok_current = { token::Type::LPAREN, current_token };
                    stateNext = TokeniserState::COMPLETE;
                }
                break;

                case TokeniserState::RPAREN: {
                    current_token += *charNow;
                    ++charNow;

                    --paren_balance;
                    tok_current = { token::Type::RPAREN, current_token };
                    stateNext = TokeniserState::COMPLETE;
                }
                break;

                case TokeniserState::LBRACE: {
                    current_token += *charNow;
                    ++charNow;

                    ++brace_balance;
                    tok_current = { token::Type::LBRACE, current_token };
                    stateNext = TokeniserState::COMPLETE;
                }
                break;

                case TokeniserState::RBRACE: {
                    current_token += *charNow;
                    ++charNow;

                    --brace_balance;
                    tok_current = { token::Type::RBRACE, current_token };
                    stateNext = TokeniserState::COMPLETE;
                }
                break;

                case TokeniserState::SEMI: {
                    current_token = *charNow;
                    tok_current = { token::Type::SEMI, current_token };
                    stateNext = TokeniserState::COMPLETE;
                    ++charNow;
                }
                break;

                case TokeniserState::COMMA: {
                    current_token = *charNow;
                    tok_current = { token::Type::COMMA, current_token };
                    stateNext = TokeniserState::COMPLETE;
                    ++charNow;
                }
                break;

                case TokeniserState::COMPLETE: {
                    output_tokens.push_back(tok_current);
                    stateNext = TokeniserState::NEW;
                }
                break;
            }

            stateNow = stateNext;
        }

        if (stateNow != TokeniserState::NEW)
            throw CompileError("[PARSER] Incomplete token '" + current_token + "'");

        if (paren_balance)
            throw CompileError("[PARSER] Parenthesis '(' & ')' not balanced");
        
        if (brace_balance)
            throw CompileError("[PARSER] Braces '{' & '}' not balanced");

        if (stateNow == TokeniserState::LSTRING)
            throw CompileError("[PARSER] Missing quotation mark - \"");

        return output_tokens;
    }

    double solve(const std::vector<token>& tokens) {
        std::deque<token> holding, output;
        token prev = { token::Type::LNUM };
        int pass = 0;

        for (const token& t : tokens) {
            if (t.type == token::Type::LNUM) {
                output.push_back(t);
                prev = output.back();
            } else if (t.type == token::Type::LPAREN) {
                holding.push_front(t);
                prev = holding.front();
            } else if (t.type == token::Type::RPAREN) {
                if (holding.empty())
                    throw CompileError("[SOLVER] Unexpected close parenthesis");

                while (!holding.empty() && holding.front().type != token::Type::LPAREN) {
                    output.push_back(holding.front());
                    holding.pop_front();
                }

                if (holding.empty())
                    throw CompileError("[SOLVER] No open parenthesis found");

                if (!holding.empty() && holding.front().type == token::Type::LPAREN)
                    holding.pop_front();

                prev = { token::Type::RPAREN };
            } else if (t.type == token::Type::OP) {
                std::string op_name = t.symbol;
                if (op_name == "-" || op_name == "+")
                    if ((prev.type != token::Type::LNUM && prev.type != token::Type::RPAREN) || pass == 0)
                        op_name = "u" + op_name;

                while (!holding.empty() && holding.front().type != token::Type::LPAREN) {
                    if (holding.front().type == token::Type::OP) {
                        const op& holding_stack_op = ops[holding.front().symbol];
                        if (holding_stack_op.precedence >= ops[op_name].precedence) {
                            output.push_back(holding.front());
                            holding.pop_front();
                        } else
                            break;
                    }
                }

                holding.push_front({ token::Type::OP, op_name });
                prev = holding.front();
            } else {
                throw CompileError("[SOLVER] Expression contains elements that can't be solved");
            }

            ++pass;
        }

        while (!holding.empty()) {
            output.push_back(holding.front());
            holding.pop_front();
        }

        std::cout << "Expression = ";
        for (const token& t : tokens)
            std::cout << t.symbol << " ";
        std::cout << "\n";

        std::cout << "RPN = ";
        for (const token& t : output)
            std::cout << t.symbol << " ";
        std::cout << "\n";

        std::deque<double> solve;
        for (const token& t : output) {
            switch (t.type) {
                case token::Type::LNUM: {
                    solve.push_front(t.value);
                }
                break;

                case token::Type::OP: {
                    const auto& oper = ops[t.symbol];

                    std::vector<double> mem(oper.arguments);
                    for (uint8_t a = 0; a < oper.arguments; ++a) {
                        if (solve.empty()) {
                            std::cout << "Empty Stack! Bad Expression\n";
                        } else {
                            mem[a] = solve[0];
                            solve.pop_front();
                        }
                    }

                    double result = 0.0;
                    if (oper.arguments == 2) {
                        if (t.symbol == "/")
                            result = mem[1] / mem[0];
                        else if (t.symbol == "*")
                            result = mem[1] * mem[0];
                        else if (t.symbol == "+")
                            result = mem[1] + mem[0];
                        else if (t.symbol == "-")
                            result = mem[1] - mem[0];
                    }

                    if (oper.arguments == 1) {
                        if (t.symbol == "u+")
                            result = +mem[0];
                        else if (t.symbol == "u-")
                            result = -mem[0];
                    }

                    solve.push_front(result);
                }
                break;

                case token::Type::Unknown:
                default:
                    throw CompileError("[SOLVER] Unexpected token");
            }
        }

        if (solve.empty())
            return 0;
        else
            return solve.front();
    }
};

int main(int argc, char* argv[]) {
    std::string exp;

    do {
        std::cout << "\nSolve: ";
        std::getline(std::cin, exp);

        // exp += ';';

        try {
            Compiler compiler;
            auto tokens = compiler.parse(exp);
            for (const token& token : tokens)
                std::cout << token.str() << "\n";

            double value = compiler.solve(tokens);
            std::cout << "Result: " << value << "\n";
        } catch (CompileError& e) {
            std::cout << e.what() << std::endl;
        }
    } while (exp != "quit");

    return 0;
}
