#pragma once

namespace npasm::lexer {

    /// Defines the type of token
    enum class TokenType {
        NEWLINE,
        COLON,
        SEMICOLON,
        NON_NEWLINE,
        MNEMONIC,
        WORD_SIZE,
        COMMA,
        REGISTER,
        BINARY_DIGIT,
        OCTAL_DIGIT,
        DECIMAL_DIGIT,
        HEX_DIGIT,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        XPLUS,
        XMINUS,
        YPLUS,
        YMINUS,
        SIMPLE_CHAR,
        ESCAPED_CONTROL_CHAR,
        ESCAPED_OCTAL_CHAR,
        ESCAPED_HEX_CHAR,

        IDENTIFIER,
        LABEL,
        COMMENT,

        BINARY_LITERAL,
        OCTAL_LITERAL,
        DECIMAL_LITERAL,
        HEX_LITERAL,
        CHAR_LITERAL,

        // These aren't implemented yet
        PERCENT, // Used for macros and other directives
        PERIOD, // Used for local labels
        PERIOD2, // Double-period used for macro-local labels
        DIRECTIVE, // A directive statement
    };

    /// The base class of all token types
    class Token {
    public:
        TokenType Type;
        std::size_t LineNumber;
        std::string_view FileName;
        inline Token(std::string const& file_name, std::size_t line_number, TokenType type) :
            Type{type}, LineNumber{line_number}, FileName{file_name} { }
        virtual inline ~Token() { }
        virtual std::string ToString() const = 0;
    };

    /// A shared pointer to a Token
    using TokenPtr = std::shared_ptr<Token>;

/// Define a simple Token type
#define TOKEN(name__) \
    class name__ : public Token { \
    public: \
        inline name__(std::string const& file_name, std::size_t line_number) : Token{file_name, line_number, TokenType::name__} { } \
        virtual inline ~name__() { } \
        virtual inline std::string ToString() const override { \
            using namespace std; \
            return (boost::format("<%s>") % #name__).str(); \
        } \
    };

/// Define a Token type that also takes a parameter
#define TOKEN1(name__, item_type__, item_name__) \
    class name__ : public Token { \
    public: \
        item_type__ item_name__; \
        inline name__(std::string const& file_name, std::size_t line_number, item_type__ const& item_name__) : Token{file_name, line_number, TokenType::name__}, item_name__{item_name__} { } \
        virtual inline ~name__() { } \
        virtual inline std::string ToString() const override { \
            using namespace std; \
            return (boost::format("<%s:%s=%s>") % #name__ % #item_name__ % boost::lexical_cast<std::string>(item_name__)).str(); \
        } \
    };

/// Define a Token type that takes a parameter and calculates how many characters of the source it consumed
#define TOKENSZ(name__, item_type__, item_name__) \
    class name__ : public Token { \
    public: \
        item_type__ item_name__; \
        std::size_t length; \
        inline name__(std::string const& file_name, std::size_t line_number, item_type__ const& item_name__, std::size_t length) : Token{file_name, line_number, TokenType::name__}, item_name__{item_name__}, length{length} { } \
        virtual inline ~name__() { } \
        virtual inline std::string ToString() const override { \
            using namespace std; \
            return (boost::format("<%s:%s=%s#%d>") % #name__ % #item_name__ % boost::lexical_cast<std::string>(item_name__) % length).str(); \
        } \
    };

    
    TOKEN(NEWLINE);
    
    TOKEN(COLON);
    
    TOKEN(SEMICOLON);
    
    TOKEN(NON_NEWLINE);
    
    TOKEN1(MNEMONIC, std::string, mnemonic);
    
    TOKEN1(WORD_SIZE, std::string, word_size);
    
    TOKEN(COMMA);
    
    TOKEN1(REGISTER, std::string, reg);
    
    TOKEN1(BINARY_DIGIT, std::string, digit);
    
    TOKEN1(OCTAL_DIGIT, std::string, digit);
    
    TOKEN1(DECIMAL_DIGIT, std::string, digit);
    
    TOKEN1(HEX_DIGIT, std::string, digit);
    
    TOKEN(LEFT_BRACKET);
    
    TOKEN(RIGHT_BRACKET);
    
    TOKEN(XPLUS);
    
    TOKEN(XMINUS);
    
    TOKEN(YPLUS);
    
    TOKEN(YMINUS);
    
    TOKEN1(SIMPLE_CHAR, std::string, ch);
    
    TOKEN1(ESCAPED_CONTROL_CHAR, std::string, ch);
    
    TOKEN1(ESCAPED_OCTAL_CHAR, std::string, ch);
    
    TOKEN1(ESCAPED_HEX_CHAR, std::string, ch);
    
    TOKEN1(IDENTIFIER, std::string, ident);
    
    TOKENSZ(LABEL, std::string, label);
    
    TOKEN1(COMMENT, std::string, comment);
    
    TOKENSZ(BINARY_LITERAL, std::int64_t, value);
    
    TOKENSZ(OCTAL_LITERAL, std::int64_t, value);
    
    TOKENSZ(DECIMAL_LITERAL, std::int64_t, value);
    
    TOKENSZ(HEX_LITERAL, std::int64_t, value);
    
    TOKENSZ(CHAR_LITERAL, char, ch);

}
