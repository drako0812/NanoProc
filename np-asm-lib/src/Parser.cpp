#include "stdafx.h"
#include "Tokens.hpp"
#include "Nodes.hpp"
#include "Parser.hpp"

namespace npasm::parser {
    using namespace std;
    using namespace boost::locale;
    using namespace npasm::lexer;

    Parser::Parser() : LastError{""} { }

    Parser::~Parser() { }

    Program::sptr Parser::Parse(TokenList const & tokens) {
        return ParseProgram(tokens);
    }

    Program::sptr Parser::ParseProgram(TokenList const & tokens) {
        auto program = Program::sptr{new Program()};

        for(auto iter = std::begin(tokens);
            iter != std::end(tokens);) {

            LastError = "";

            if(auto [line, next] = ParseLine(tokens, iter); line != nullptr) {
                program->Lines.push_back(line);
                iter = next;
            } else {
                if(iter != std::end(tokens)) {
                    throw std::exception(
                        (
                            "PARSER: Unable to parse line " +
                            string((*iter)->FileName) + ":" +
                            to_string((*iter)->LineNumber) + "\n" +
                            LastError
                       ).c_str()
                    );
                } else {
                    throw std::exception(
                        (
                            "PARSER: Unable to parse line\n" +
                            LastError
                        ).c_str()
                    );
                }
            }
        }

        return program;
    }

    Parser::ParseReturn<Line::sptr> Parser::ParseLine(TokenList const & tokens, TokenList::const_iterator cpos) {
        auto line = Line::sptr{new Line()};

        if(auto[label, next] = ParseLabel(tokens, cpos); label != nullptr) {
            line->Label = label;
            cpos = next;
        }

        if(auto[instruction, next] = ParseInstruction(tokens, cpos); instruction != nullptr) {
            line->Instruction = instruction;
            cpos = next;
        }

        if(auto[comment, next] = ParseComment(tokens, cpos); comment != nullptr) {
            line->Comment = comment;
            cpos = next;
        }

        if(auto newline = *cpos; newline->Type != TokenType::NEWLINE) {
            LastError += "Unable to parse Newline\n";
            return std::make_tuple(nullptr, cpos);
        }

        return std::make_tuple(line, cpos);
    }

    Parser::ParseReturn<Label::sptr> Parser::ParseLabel(TokenList const & tokens, TokenList::const_iterator cpos) {
        auto tok = *cpos;

        if(tok->Type == TokenType::LABEL) {
            auto label_tok = static_pointer_cast<LABEL>(tok);
            auto label = Label::sptr(new Label(label_tok->label));
            cpos++;
            return make_tuple(label, cpos);
        } else {
            LastError += "Unable to parse Label\n";
            return make_tuple(nullptr, cpos);
        }
    }

    const set<std::string> NoArgOps = {
        "nop", "pop", "ret", "iret", "iqe", "iqd", "ion", "ioff", "halt"
    };
    const set<std::string> OneArgOps = {
        "inc", "dec", "cmpz", "cmpnz", "stack", "push",
        "jump", "call", "jumpc", "callc", "int", "irset",
        "hwnum", "hwqry", "hwint"
    };
    const set<std::string> TwoArgOps = {
        "move", "swap", "add", "sub", "mul", "muls", "div", "divs",
        "mod", "mods", "and", "bor", "xor", "shr", "shra", "shl",
        "cmpeq", "cmpne", "cmpgt", "cmplt", "cmpge", "cmple",
        "cmpgts", "cmplts", "cmpges", "cmples", "iset", "hwin",
        "hwout"
    };
    const set<std::string> OpsWithoutSize = {
        "nop", "jump", "call", "ret", "jumpc", "callc",
        "iret", "iqe", "iqd", "ion", "ioff", "hwqry", "halt"
    };

    Parser::ParseReturn<Instruction::sptr> Parser::ParseInstruction(TokenList const & tokens, TokenList::const_iterator cpos) {
        auto tok = *cpos;

        auto[mnemonic, next] = ParseMnemonic(tokens, cpos);
        if(mnemonic == nullptr) {
            LastError += "Unable to parse Instruction\n";
            return make_tuple(nullptr, cpos);
        }
        cpos = next;
        auto mnemonic_str = to_lower(mnemonic->Value);

        auto[word_size, next] = ParseWordSize(tokens, cpos);
        if(word_size != nullptr) { // If it has a WordSize
            cpos = next;
            if(OpsWithoutSize.count(mnemonic_str) != 0) { // And Shouldn't have one
                // Malformed instruction
                LastError += "Word Size given for an instruction that doesn't need it (" + mnemonic_str + " | " + to_lower(word_size->Value) + ")\n";
                return make_tuple(nullptr, cpos);
            } else { // Does have one
                // Nothing extra to do we fall out to the next step
            }
        } else { // No Word Size
            // Everything can ommit a Word Size
        }

        if(NoArgOps.count(mnemonic_str) != 0) { // No arg opcode
            if(auto[inst, next] = ParseNoArgumentInstruction(tokens, cpos); inst != nullptr) {
                inst->Mnemonic = mnemonic;
                inst->WordSize = word_size;
                cpos = next;
                return make_tuple(inst, cpos);
            } else {
                LastError += "Unable to parse No-Argument instruction (" + mnemonic_str + ")\n";
                return make_tuple(nullptr, cpos);
            }
        } else if(OneArgOps.count(mnemonic_str) != 0) { // One arg opcode
            if(auto[inst, next] = ParseOneArgumentInstruction(tokens, cpos); inst != nullptr) {
                inst->Mnemonic = mnemonic;
                inst->WordSize = word_size;
                cpos = next;
                return make_tuple(inst, cpos);
            } else {
                LastError += "Unable to parse One-Argument instruction (" + mnemonic_str + ")\n";
                return make_tuple(nullptr, cpos);
            }
        } else if(TwoArgOps.count(mnemonic_str) != 0) { // Two arg opcode
            if(auto[inst, next] = ParseTwoArgumentInstruction(tokens, cpos); inst != nullptr) {
                inst->Mnemonic = mnemonic;
                inst->WordSize = word_size;
                cpos = next;
                return make_tuple(inst, cpos);
            } else {
                LastError += "Unable to parse Two-Argument instruction (" + mnemonic_str + ")\n";
                return make_tuple(nullptr, cpos);
            }
        } else { // Unknown mnemonic
            LastError += "Unknown Mnemonic (" + mnemonic_str + ")\n";
            return make_tuple(nullptr, cpos);
        }
    }

    Parser::ParseReturn<Comment::sptr> Parser::ParseComment(TokenList const & tokens, TokenList::const_iterator cpos) {
        auto tok = *cpos;

        if(tok->Type == TokenType::COMMENT) {
            // TODO: Finish
        }
    }

    Parser::ParseReturn<TwoArgumentInstruction::sptr> Parser::ParseTwoArgumentInstruction(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<TwoArgumentInstruction::sptr>();
    }

    Parser::ParseReturn<OneArgumentInstruction::sptr> Parser::ParseOneArgumentInstruction(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<OneArgumentInstruction::sptr>();
    }

    Parser::ParseReturn<IdentifierArgument::sptr> Parser::ParseIdentifierArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<IdentifierArgument::sptr>();
    }

    Parser::ParseReturn<IntegerArgument::sptr> Parser::ParseIntegerArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<IntegerArgument::sptr>();
    }

    Parser::ParseReturn<ImmediateArgument::sptr> Parser::ParseImmediateArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<ImmediateArgument::sptr>();
    }

    Parser::ParseReturn<RegisterArgument::sptr> Parser::ParseRegisterArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<RegisterArgument::sptr>();
    }

    Parser::ParseReturn<YIndexArgument::sptr> Parser::ParseYIndexArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<YIndexArgument::sptr>();
    }

    Parser::ParseReturn<XIndexArgument::sptr> Parser::ParseXIndexArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<XIndexArgument::sptr>();
    }

    Parser::ParseReturn<PointerArgument::sptr> Parser::PointerArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<PointerArgument::sptr>();
    }

    Parser::ParseReturn<Argument::sptr> Parser::ParseArgument(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<Argument::sptr>();
    }

    Parser::ParseReturn<NoArgumentInstruction::sptr> Parser::ParseNoArgumentInstruction(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<NoArgumentInstruction::sptr>();
    }

    Parser::ParseReturn<WordSize::sptr> Parser::ParseWordSize(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<WordSize::sptr>();
    }

    Parser::ParseReturn<Mnemonic::sptr> Parser::ParseMnemonic(TokenList const & tokens, TokenList::const_iterator cpos) {
        return Parser::ParseReturn<Mnemonic::sptr>();
    }

    
}
