#pragma once

namespace npasm::parser {

    /// Parses a list of Tokens into a syntax tree
    class Parser {
    public:
        Parser();
        ~Parser();

        using TokenList = std::vector<lexer::TokenPtr>;

        template<class T>
        using ParseReturn = std::tuple<T, TokenList::const_iterator>;

        Program::sptr Parse(TokenList const& tokens);

        Program::sptr ParseProgram(TokenList const& tokens);
        ParseReturn<Line::sptr> ParseLine(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<Label::sptr> ParseLabel(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<Instruction::sptr> ParseInstruction(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<Comment::sptr> ParseComment(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<TwoArgumentInstruction::sptr> ParseTwoArgumentInstruction(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<OneArgumentInstruction::sptr> ParseOneArgumentInstruction(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<IdentifierArgument::sptr> ParseIdentifierArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<IntegerArgument::sptr> ParseIntegerArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<ImmediateArgument::sptr> ParseImmediateArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<RegisterArgument::sptr> ParseRegisterArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<YIndexArgument::sptr> ParseYIndexArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<XIndexArgument::sptr> ParseXIndexArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<PointerArgument::sptr> PointerArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<Argument::sptr> ParseArgument(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<NoArgumentInstruction::sptr> ParseNoArgumentInstruction(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<WordSize::sptr> ParseWordSize(TokenList const& tokens, TokenList::const_iterator cpos);
        ParseReturn<Mnemonic::sptr> ParseMnemonic(TokenList const& tokens, TokenList::const_iterator cpos);
        
    private:
        std::string LastError;
    };

}
