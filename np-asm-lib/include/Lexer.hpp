#pragma once

namespace npasm::lexer {

    /// Tokenizes a string of source code.
    class Lexer {
    public:
        Lexer();
        ~Lexer();

        //using MaybeStr = std::optional<std::string>;
        std::vector<TokenPtr> LexFile(std::string const& str);
        std::vector<TokenPtr> LexString(std::string const& str);

        TokenPtr IsNEWLINE(std::string const& str);
        TokenPtr IsCOLON(std::string const& str);
        TokenPtr IsSEMICOLON(std::string const& str);
        TokenPtr IsNON_NEWLINE(std::string const& str);
        TokenPtr IsMNEMONIC(std::string const& str);
        TokenPtr IsWORD_SIZE(std::string const& str);
        TokenPtr IsCOMMA(std::string const& str);
        TokenPtr IsREGISTER(std::string const& str);
        TokenPtr IsBINARY_DIGIT(std::string const& str);
        TokenPtr IsOCTAL_DIGIT(std::string const& str);
        TokenPtr IsDECIMAL_DIGIT(std::string const& str);
        TokenPtr IsHEX_DIGIT(std::string const& str);
        TokenPtr IsLEFT_BRACKET(std::string const& str);
        TokenPtr IsRIGHT_BRACKET(std::string const& str);
        TokenPtr IsXPLUS(std::string const& str);
        TokenPtr IsXMINUS(std::string const& str);
        TokenPtr IsYPLUS(std::string const& str);
        TokenPtr IsYMINUS(std::string const& str);
        TokenPtr IsSIMPLE_CHAR(std::string const& str);
        TokenPtr IsESCAPED_CONTROL_CHAR(std::string const& str);
        TokenPtr IsESCAPED_OCTAL_CHAR(std::string const& str);
        TokenPtr IsESCAPED_HEX_CHAR(std::string const& str);

        TokenPtr IsIDENTIFIER(std::string const& str);
        TokenPtr IsLABEL(std::string const& str);
        TokenPtr IsCOMMENT(std::string const& str);

        TokenPtr IsBINARY_LITERAL(std::string const& str);
        TokenPtr IsOCTAL_LITERAL(std::string const& str);
        TokenPtr IsDECIMAL_LITERAL(std::string const& str);
        TokenPtr IsHEX_LITERAL(std::string const& str);

        TokenPtr IsCHAR_LITERAL(std::string const& str);

        size_t SkipWhitespace(std::string const& str);
        std::int64_t FromBinary(std::string const& str);
        std::int64_t FromOctal(std::string const& str);
        std::int64_t FromHex(std::string const& str);

    private:
        std::string FileName;
        std::size_t CurrentLine;

        std::vector<TokenPtr> LexStringIntern(std::string const& str);
    };

    /// Converts a list of Tokens into a debug string
    std::string ToString(std::vector<TokenPtr> const& tokens);

}

/// \}
