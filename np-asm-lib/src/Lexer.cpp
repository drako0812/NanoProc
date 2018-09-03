#include "stdafx.h"
#include "Tokens.hpp"
#include "Lexer.hpp"

namespace npasm::lexer {

    using namespace std;
    namespace rc = std::regex_constants;

    Lexer::Lexer() : FileName{""}, CurrentLine{0} { }

    Lexer::~Lexer() { }

    std::vector<TokenPtr> Lexer::LexFile(std::string const & str) {
        std::ifstream inf(str);
        if(inf) {
            std::ostringstream contents;
            contents << inf.rdbuf();
            inf.close();
            FileName = str;
            CurrentLine = 0;
            return LexStringIntern(contents.str());
        }
        throw std::exception("Unable to open source file");
    }

    std::vector<TokenPtr> Lexer::LexString(std::string const & str) {
        FileName = "__LEXED_STRING__";
        CurrentLine = 0;
        return LexStringIntern(str);
    }

    vector<TokenPtr> Lexer::LexStringIntern(string const & str) {
        vector<TokenPtr> tokens = {};
        size_t start_idx = 0;

        while(start_idx < str.length()) {

            if((str[start_idx] == ' ') || // Ignore non-newline whitespace
                (str[start_idx] == '\t') ||
                (str[start_idx] == '\v') ||
                (str[start_idx] == '\a') ||
                (str[start_idx] == '\b') ||
                (str[start_idx] == '\f') ||
                (str[start_idx] == '\r')) {
                start_idx++;
                continue;
            }

            auto tok = IsNEWLINE(str.substr(start_idx));

            if(tok != nullptr) { // Success
                tokens.push_back(tok);
                start_idx++;
                CurrentLine++;
                continue;
            }

            if(tok = IsCOLON(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx++;
                continue;
            }

            if(tok = IsCOMMENT(str.substr(start_idx)); tok != nullptr) {
                auto comment = static_pointer_cast<COMMENT>(tok);
                tokens.push_back(tok);
                start_idx += comment->comment.length() + 1;
                continue;
            }

            if(tok = IsSEMICOLON(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx++;
                continue;
            }

            /*if(tok = IsNON_NEWLINE(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx++;
                continue;
            }*/

            if(tok = IsMNEMONIC(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += static_pointer_cast<MNEMONIC>(tok)->mnemonic.length();
                continue;
            }

            if(tok = IsWORD_SIZE(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += static_pointer_cast<WORD_SIZE>(tok)->word_size.length();
                continue;
            }

            if(tok = IsCOMMA(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx++;
                continue;
            }

            if(tok = IsREGISTER(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += static_pointer_cast<REGISTER>(tok)->reg.length();
                continue;
            }

            if(tok = IsLEFT_BRACKET(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx++;
                continue;
            }

            if(tok = IsRIGHT_BRACKET(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx++;
                continue;
            }

            if(tok = IsXPLUS(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += 2;
                continue;
            }

            if(tok = IsXMINUS(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += 2;
                continue;
            }

            if(tok = IsYPLUS(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += 2;
                continue;
            }

            if(tok = IsYMINUS(str.substr(start_idx)); tok != nullptr) {
                tokens.push_back(tok);
                start_idx += 2;
                continue;
            }

            if(tok = IsLABEL(str.substr(start_idx)); tok != nullptr) {
                auto l = static_pointer_cast<LABEL>(tok);
                tokens.push_back(tok);
                start_idx += l->length;
                continue;
            }

            if(tok = IsIDENTIFIER(str.substr(start_idx)); tok != nullptr) {
                auto ident = static_pointer_cast<IDENTIFIER>(tok);
                tokens.push_back(tok);
                start_idx += ident->ident.length();
                continue;
            }

            if(tok = IsDECIMAL_LITERAL(str.substr(start_idx)); tok != nullptr) {
                auto lit = static_pointer_cast<DECIMAL_LITERAL>(tok);
                tokens.push_back(tok);
                start_idx += lit->length;
                continue;
            }

            if(tok = IsBINARY_LITERAL(str.substr(start_idx)); tok != nullptr) {
                auto lit = static_pointer_cast<BINARY_LITERAL>(tok);
                tokens.push_back(tok);
                start_idx += lit->length;
                continue;
            }

            if(tok = IsHEX_LITERAL(str.substr(start_idx)); tok != nullptr) {
                auto lit = static_pointer_cast<HEX_LITERAL>(tok);
                tokens.push_back(tok);
                start_idx += lit->length;
                continue;
            }

            if(tok = IsOCTAL_LITERAL(str.substr(start_idx)); tok != nullptr) {
                auto lit = static_pointer_cast<OCTAL_LITERAL>(tok);
                tokens.push_back(tok);
                start_idx += lit->length;
                continue;
            }

            if(tok = IsCHAR_LITERAL(str.substr(start_idx)); tok != nullptr) {
                auto lit = static_pointer_cast<CHAR_LITERAL>(tok);
                tokens.push_back(tok);
                start_idx += lit->length;
                continue;
            }

            start_idx++;
        }

        return tokens;
    }

    TokenPtr Lexer::IsNEWLINE(string const& str) {
        static regex re = regex(R"(\n)");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<NEWLINE>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsCOLON(string const& str) {
        static regex re = regex(":");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<COLON>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsSEMICOLON(string const& str) {
        static regex re = regex(";");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<SEMICOLON>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsNON_NEWLINE(string const& str) {
        static regex re = regex(R"([^\n])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<NON_NEWLINE>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsMNEMONIC(string const & str) {
        static regex re = regex(boost::erase_all_copy(
            R"(NOP|
MOVE|
SWAP|
ADD|
SUB|
INC|
DEC|
MULS|
MUL|
DIVS|
DIV|
MODS|
MOD|
AND|
BOR|
XOR|
SHRA|
SHR|
SHL|
CMPZ|
CMPNZ|
CMPEQ|
CMPNE|
CMPGTS|
CMPLTS|
CMPGES|
CMPLES|
CMPGT|
CMPLT|
CMPGE|
CMPLE|
STACK|
PUSH|
POP|
JUMPC|
CALLC|
JUMP|
CALL|
RET|
INT|
IRET|
IQE|
IQD|
ISET|
IRSET|
ION|
IOFF|
HWIN|
HWOUT|
HWNUM|
HWQRY|
HWINT|
HALT)"s,
"\n"s
), rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<MNEMONIC>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsWORD_SIZE(string const & str) {
        static regex re = regex(R"(word|byte)", rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<WORD_SIZE>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsCOMMA(string const& str) {
        static regex re = regex(",");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<COMMA>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsREGISTER(string const & str) {
        static regex re = regex(boost::erase_all_copy(
            R"(\$((ACC|COMP|EXC|INTQ|INT|ION|STL|SP|PC|A|B|C|D|E|F|G|H|X|Y)((l(l|h)?)|(h(l|h)?))?))"s,
            "\n"s), rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<REGISTER>(FileName, CurrentLine, m.str(1));
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsBINARY_DIGIT(string const& str) {
        static regex re = regex(R"([01])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<BINARY_DIGIT>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsOCTAL_DIGIT(string const& str) {
        static regex re = regex(R"([0-7])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<OCTAL_DIGIT>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsDECIMAL_DIGIT(string const& str) {
        static regex re = regex(R"([0-9])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<DECIMAL_DIGIT>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsHEX_DIGIT(string const& str) {
        static regex re = regex(R"([0-9a-fA-F])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<HEX_DIGIT>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsLEFT_BRACKET(string const& str) {
        static regex re = regex(R"(\[)");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<LEFT_BRACKET>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsRIGHT_BRACKET(string const& str) {
        static regex re = regex(R"(\])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<RIGHT_BRACKET>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsXPLUS(string const & str) {
        static regex re = regex(R"(X\+)", rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<XPLUS>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsXMINUS(string const & str) {
        static regex re = regex(R"(X-)", rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<XMINUS>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsYPLUS(string const & str) {
        static regex re = regex(R"(Y\+)", rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<YPLUS>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsYMINUS(string const & str) {
        static regex re = regex(R"(Y-)", rc::icase);
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<YMINUS>(FileName, CurrentLine);
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsSIMPLE_CHAR(string const& str) {
        static regex re = regex(R"([\x20-\x26\x28-\x5b\x5d-\xfe])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<SIMPLE_CHAR>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsESCAPED_CONTROL_CHAR(string const & str) {
        static regex re = regex(R"(\\[abfnrtv'0\\])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<ESCAPED_CONTROL_CHAR>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsESCAPED_OCTAL_CHAR(string const & str) {
        static regex re = regex(R"(\\[0-7][0-7][0-7])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<ESCAPED_OCTAL_CHAR>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsESCAPED_HEX_CHAR(string const & str) {
        static regex re = regex(R"(\\x[0-9a-fA-F][0-9a-fA-F])");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<ESCAPED_HEX_CHAR>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsIDENTIFIER(std::string const & str) {
        static regex re = regex(R"([a-zA-Z_][0-9a-zA-Z_]*)");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<IDENTIFIER>(FileName, CurrentLine, m.str());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsLABEL(std::string const & str) {
        // This is slightly more complicated we scan for two tokens in sequence
        auto ident = IsIDENTIFIER(str);
        if(ident == nullptr) {
            return nullptr;
        }
        size_t idx = static_pointer_cast<IDENTIFIER>(ident)->ident.length();
        idx += SkipWhitespace(str.substr(idx));
        auto colon = IsCOLON(str.substr(idx));
        if(colon == nullptr) {
            return nullptr;
        }
        return make_shared<LABEL>(FileName, CurrentLine, static_pointer_cast<IDENTIFIER>(ident)->ident, idx + 1);
    }

    TokenPtr Lexer::IsCOMMENT(std::string const& str) {
        static regex re = regex(R"(;([^\n]*))");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            return make_shared<COMMENT>(FileName, CurrentLine, m.str(1));
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsBINARY_LITERAL(std::string const & str) {
        static regex re = regex(R"(([\+-]?)0b([01]+))");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {

            auto value = FromBinary(m.str(2));
            if(m.str(1) == "-") {
                value *= -1;
            }

            return make_shared<BINARY_LITERAL>(FileName, CurrentLine, value, m.str(0).length());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsOCTAL_LITERAL(std::string const & str) {
        static regex re = regex(R"(([\+-]?)(0[0-7]*))");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {

            std::int64_t value;

            if(m.str(2) == "0") {
                value = 0;
            } else {
                value = FromOctal(m.str(2));
                if(m.str(1) == "-") {
                    value *= -1;
                }
            }

            return make_shared<OCTAL_LITERAL>(FileName, CurrentLine, value, m.str(0).length());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsDECIMAL_LITERAL(std::string const & str) {
        static regex re = regex(R"(([\+-]?)([1-9][0-9]*))");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {
            using namespace boost;
            using namespace boost::cnv;
            static boost::cnv::cstream cnv;

            auto value = convert<std::int64_t>(m.str(1) + m.str(2), cnv(base::dec)).value();

            return make_shared<DECIMAL_LITERAL>(FileName, CurrentLine, value, m.str(0).length());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsHEX_LITERAL(std::string const & str) {
        static regex re = regex(R"(([\+-]?)0[xX]([0-9a-fA-F]+))");
        smatch m;
        bool found = regex_search(str, m, re, rc::match_continuous);
        if(found) {

            auto value = FromHex(m.str(2));
            if(m.str(1) == "-") {
                value *= -1;
            }

            return make_shared<HEX_LITERAL>(FileName, CurrentLine, value, m.str(0).length());
        } else {
            return nullptr;
        }
    }

    TokenPtr Lexer::IsCHAR_LITERAL(std::string const & str) {
        size_t idx = 0;
        if(str[idx++] != '\'') {
            return nullptr;
        }

        auto tok = IsSIMPLE_CHAR(str.substr(idx));
        if(tok != nullptr) {
            idx++;
            if(str[idx] != '\'') {
                return nullptr;
            }
            return make_shared<CHAR_LITERAL>(FileName, CurrentLine, static_pointer_cast<SIMPLE_CHAR>(tok)->ch[0], 3);
        }

        tok = IsESCAPED_HEX_CHAR(str.substr(idx));
        if(tok != nullptr) {
            idx++;
            if(str[idx] != '\'') {
                return nullptr;
            }
            using namespace boost;
            using namespace boost::cnv;
            static boost::cnv::cstream cnv;

            auto c_s = static_pointer_cast<ESCAPED_HEX_CHAR>(tok)->ch.substr(2);
            char c = (char)(convert<std::uint64_t>(c_s, cnv(base::hex)).value());

            return make_shared<CHAR_LITERAL>(FileName, CurrentLine, c, 6);
        }

        tok = IsESCAPED_OCTAL_CHAR(str.substr(idx));
        if(tok != nullptr) {
            idx++;
            if(str[idx] != '\'') {
                return nullptr;
            }
            using namespace boost;
            using namespace boost::cnv;
            static boost::cnv::cstream cnv;

            auto c_s = static_pointer_cast<ESCAPED_OCTAL_CHAR>(tok)->ch.substr(1);
            char c = (char)(convert<std::uint64_t>(c_s, cnv(base::oct)).value());

            return make_shared<CHAR_LITERAL>(FileName, CurrentLine, c, 6);
        }

        tok = IsESCAPED_CONTROL_CHAR(str.substr(idx));
        if(tok != nullptr) {
            idx++;
            if(str[idx] != '\'') {
                return nullptr;
            }

            auto c_s = static_pointer_cast<ESCAPED_CONTROL_CHAR>(tok)->ch.substr(1);
            char c;

            switch(c_s[0]) {
            case 'a':
                c = '\a';
                break;
            case 'b':
                c = '\b';
                break;
            case 'f':
                c = '\f';
                break;
            case 'n':
                c = '\n';
                break;
            case 'r':
                c = '\r';
                break;
            case 't':
                c = '\t';
                break;
            case 'v':
                c = '\v';
                break;
            case '\'':
                c = '\'';
                break;
            case '0':
                c = '\0';
                break;
            case '\\':
                c = '\\';
                break;
            }

            return make_shared<CHAR_LITERAL>(FileName, CurrentLine, c, 4);
        }

        return nullptr;
    }

    size_t Lexer::SkipWhitespace(std::string const & str) {
        size_t idx = 0;
        while((str[idx] == ' ') || // Ignore non-newline whitespace
            (str[idx] == '\t') ||
            (str[idx] == '\v') ||
            (str[idx] == '\a') ||
            (str[idx] == '\b') ||
            (str[idx] == '\f') ||
            (str[idx] == '\r')) {

            idx++;
        }
        return idx;
    }

    std::int64_t Lexer::FromBinary(std::string const& str) {
        std::int64_t ret = 0;

        for(char c : str) {
            switch(c) {
            case '0':
                ret <<= 1;
                ret &= 0xFFFFFFFFFFFFFFFE;
                break;
            case '1':
                ret <<= 1;
                ret |= 0x1;
                break;
            default:
                throw std::exception("Not a binary digit");
                break;
            }
        }

        return ret;
    }

    std::int64_t Lexer::FromOctal(std::string const& str) {
        std::int64_t ret = 0;

        for(char c : str) {
            ret *= 8;
            switch(c) {
            case '0':
                ret += 0;
                break;
            case '1':
                ret++;
                break;
            case '2':
                ret += 2;
                break;
            case '3':
                ret += 3;
                break;
            case '4':
                ret += 4;
                break;
            case '5':
                ret += 5;
                break;
            case '6':
                ret += 6;
                break;
            case '7':
                ret += 7;
                break;
            default:
                throw std::exception("Not an octal digit");
                break;
            }
        }
        return ret;
    }

    std::int64_t Lexer::FromHex(std::string const& str) {
        using namespace std;
        static map<char, int64_t> hexm = {
            { '0', 0x0 }, { '1', 0x1 }, { '2', 0x2 }, { '3', 0x3 }, { '4', 0x4 },
            { '5', 0x5 }, { '6', 0x6 }, { '7', 0x7 }, { '8', 0x8 }, { '9', 0x9 },
            { 'a', 0xa }, { 'b', 0xb }, { 'c', 0xc }, { 'd', 0xd }, { 'e', 0xe }, { 'f', 0xf },
            { 'A', 0xa }, { 'B', 0xb }, { 'C', 0xc }, { 'D', 0xd }, { 'E', 0xe }, { 'F', 0xf },
        };
        std::int64_t ret = 0;

        for(char c : str) {
            ret *= 16;
            ret += hexm[c];
        }

        return ret;
    }

    std::string ToString(std::vector<TokenPtr> const & tokens) {
        std::string ret = "";

        for(auto const& tok : tokens) {
            ret += tok->ToString();
        }

        return ret;
    }

}
