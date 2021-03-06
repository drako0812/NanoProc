#include "pch.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "Tokens.hpp"
#include "Lexer.hpp"

constexpr char source[] = R"(
            MOVE byte $All,            0xfe ; Set All to 254
            MOVE word  $Bl,  [X+0xfedcba98] ; Set Bl to value in memory at X + 4,275,878,552
            MOVE      $ACC,               0 ; Initialize register ACC to 0
loop_begin: CMPGE     $ACC,             100 ; Compare if register ACC is greater than or equal
                                            ; to 100
            JUMPC     loop_end              ; If the previous condition was true jump to the end
                                            ; of the loop
            INC       $ACC                  ; Increment register ACC by 1
            JUMP      loop_begin            ; Jump to beginning of loop
loop_end:   HALT                            ; Halt CPU execution
)";

TEST_CASE("Testing the Lexer", "[lexer]") {
    using namespace std;
    using namespace npasm;
    auto l = Lexer();

    SECTION("IsNEWLINE") {
        REQUIRE(l.IsNEWLINE("\n") != nullptr);
        REQUIRE(l.IsNEWLINE(" ") == nullptr);
    }

    SECTION("IsCOLON") {
        REQUIRE(l.IsCOLON(":") != nullptr);
        REQUIRE(l.IsCOLON(" ") == nullptr);
    }

    SECTION("IsSEMICOLON") {
        REQUIRE(l.IsSEMICOLON(";") != nullptr);
        REQUIRE(l.IsSEMICOLON(" ") == nullptr);
    }

    SECTION("IsNON_NEWLINE") {
        REQUIRE(l.IsNON_NEWLINE("a") != nullptr);
        REQUIRE(l.IsNON_NEWLINE("\n") == nullptr);
    }

    SECTION("IsWORD_SIZE") {
        REQUIRE(l.IsWORD_SIZE("word") != nullptr);
        REQUIRE(l.IsWORD_SIZE("byte") != nullptr);
        REQUIRE(l.IsWORD_SIZE("dword") == nullptr);
    }

    SECTION("IsCOMMA") {
        REQUIRE(l.IsCOMMA(",") != nullptr);
        REQUIRE(l.IsCOMMA(" ") == nullptr);
    }

    SECTION("IsBINARY_DIGIT") {
        REQUIRE(l.IsBINARY_DIGIT("0") != nullptr);
        REQUIRE(l.IsBINARY_DIGIT("1") != nullptr);
        REQUIRE(l.IsBINARY_DIGIT("2") == nullptr);
    }

    SECTION("IsOCTAL_DIGIT") {
        REQUIRE(l.IsOCTAL_DIGIT("0") != nullptr);
        REQUIRE(l.IsOCTAL_DIGIT("7") != nullptr);
        REQUIRE(l.IsOCTAL_DIGIT("8") == nullptr);
    }

    SECTION("IsDECIMAL_DIGIT") {
        REQUIRE(l.IsDECIMAL_DIGIT("0") != nullptr);
        REQUIRE(l.IsDECIMAL_DIGIT("9") != nullptr);
        REQUIRE(l.IsDECIMAL_DIGIT("a") == nullptr);
    }

    SECTION("IsHEX_DIGIT") {
        REQUIRE(l.IsHEX_DIGIT("0") != nullptr);
        REQUIRE(l.IsHEX_DIGIT("9") != nullptr);
        REQUIRE(l.IsHEX_DIGIT("a") != nullptr);
        REQUIRE(l.IsHEX_DIGIT("F") != nullptr);
        REQUIRE(l.IsHEX_DIGIT("g") == nullptr);
    }

    SECTION("IsLEFT_BRACKET") {
        REQUIRE(l.IsLEFT_BRACKET("[") != nullptr);
        REQUIRE(l.IsLEFT_BRACKET(" ") == nullptr);
    }

    SECTION("IsRIGHT_BRACKET") {
        REQUIRE(l.IsRIGHT_BRACKET("]") != nullptr);
        REQUIRE(l.IsRIGHT_BRACKET(" ") == nullptr);
    }

    SECTION("IsXPLUS") {
        REQUIRE(l.IsXPLUS("X+") != nullptr);
        REQUIRE(l.IsXPLUS("X-") == nullptr);
    }

    SECTION("IsXMINUS") {
        REQUIRE(l.IsXMINUS("X-") != nullptr);
        REQUIRE(l.IsXMINUS("X+") == nullptr);
    }

    SECTION("IsYPLUS") {
        REQUIRE(l.IsYPLUS("Y+") != nullptr);
        REQUIRE(l.IsYPLUS("Y-") == nullptr);
    }

    SECTION("IsYMINUS") {
        REQUIRE(l.IsYMINUS("Y-") != nullptr);
        REQUIRE(l.IsYMINUS("Y+") == nullptr);
    }

    SECTION("IsSIMPLE_CHAR") {
        REQUIRE(l.IsSIMPLE_CHAR("a") != nullptr);
    }

    SECTION("IsESCAPED_CONTROL_CHAR") {
        REQUIRE(l.IsESCAPED_CONTROL_CHAR("\\n") != nullptr);
        REQUIRE(l.IsESCAPED_CONTROL_CHAR("\\m") == nullptr);
    }

    SECTION("IsESCAPED_OCTAL_CHAR") {
        REQUIRE(l.IsESCAPED_OCTAL_CHAR("\\765") != nullptr);
        REQUIRE(l.IsESCAPED_OCTAL_CHAR("\\876") == nullptr);
    }

    SECTION("IsESCAPED_HEX_CHAR") {
        REQUIRE(l.IsESCAPED_HEX_CHAR("\\xf9") != nullptr);
        REQUIRE(l.IsESCAPED_HEX_CHAR("\\xgg") == nullptr);
    }

    SECTION("IsMNEMONIC") {
        auto m = l.IsMNEMONIC("NOP");
        REQUIRE(m != nullptr);
        REQUIRE(static_pointer_cast<MNEMONIC>(m)->mnemonic == "NOP");

        m = l.IsMNEMONIC("HALT");
        REQUIRE(m != nullptr);
        REQUIRE(static_pointer_cast<MNEMONIC>(m)->mnemonic == "HALT");

        m = l.IsMNEMONIC("CMPGTS");
        REQUIRE(m != nullptr);
        REQUIRE(static_pointer_cast<MNEMONIC>(m)->mnemonic == "CMPGTS");

        m = l.IsMNEMONIC("CMPGT");
        REQUIRE(m != nullptr);
        REQUIRE(static_pointer_cast<MNEMONIC>(m)->mnemonic == "CMPGT");
    }

    SECTION("IsREGISTER") {
        auto r = l.IsREGISTER("$INTQ");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "INTQ");

        r = l.IsREGISTER("$INT");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "INT");

        r = l.IsREGISTER("$A");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "A");

        r = l.IsREGISTER("$Al");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "Al");

        r = l.IsREGISTER("$Ah");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "Ah");

        r = l.IsREGISTER("$All");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "All");

        r = l.IsREGISTER("$Ahl");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "Ahl");

        r = l.IsREGISTER("$Alh");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "Alh");

        r = l.IsREGISTER("$Ahh");
        REQUIRE(r != nullptr);
        REQUIRE(static_pointer_cast<REGISTER>(r)->reg == "Ahh");
    }

    SECTION("IsIDENTIFIER") {
        auto i = l.IsIDENTIFIER("_main");
        REQUIRE(i != nullptr);
        REQUIRE(static_pointer_cast<IDENTIFIER>(i)->ident == "_main");

        i = l.IsIDENTIFIER("123");
        REQUIRE(i == nullptr);
    }

    SECTION("IsLABEL") {
        auto i = l.IsLABEL("_main:");
        REQUIRE(i != nullptr);
        REQUIRE(static_pointer_cast<LABEL>(i)->label == "_main");

        i = l.IsLABEL("_main    :");
        REQUIRE(i != nullptr);
        REQUIRE(static_pointer_cast<LABEL>(i)->label == "_main");

        i = l.IsLABEL("123:");
        REQUIRE(i == nullptr);
    }

    SECTION("IsCOMMENT") {
        auto c = l.IsCOMMENT("; Hello");
        REQUIRE(c != nullptr);
        REQUIRE(static_pointer_cast<COMMENT>(c)->comment == " Hello");

        c = l.IsCOMMENT("; Hello\n");
        REQUIRE(c != nullptr);
        REQUIRE(static_pointer_cast<COMMENT>(c)->comment == " Hello");

        c = l.IsCOMMENT(" Hello");
        REQUIRE(c == nullptr);
    }

    SECTION("IsBINARY_LITERAL") {
        auto lit = l.IsBINARY_LITERAL("0b0101");
        REQUIRE(lit != nullptr);
        REQUIRE(static_pointer_cast<BINARY_LITERAL>(lit)->value == 0b0101);

        lit = l.IsBINARY_LITERAL("0b");
        REQUIRE(lit == nullptr);

        lit = l.IsBINARY_LITERAL("0101");
        REQUIRE(lit == nullptr);
    }

    SECTION("IsOCTAL_LITERAL") {
        auto lit = l.IsOCTAL_LITERAL("0765");
        REQUIRE(lit != nullptr);
        REQUIRE(static_pointer_cast<OCTAL_LITERAL>(lit)->value == 0765);

        lit = l.IsOCTAL_LITERAL("0");
        REQUIRE(lit != nullptr);
        REQUIRE(static_pointer_cast<OCTAL_LITERAL>(lit)->value == 0);

        lit = l.IsOCTAL_LITERAL("0876"); // This succeeds because IsOCTAL_LITERAL doesn't consume the entire string
        REQUIRE(lit != nullptr);
        REQUIRE(static_pointer_cast<OCTAL_LITERAL>(lit)->value == 0);

        lit = l.IsOCTAL_LITERAL("8");
        REQUIRE(lit == nullptr);
    }

    SECTION("IsDECIMAL_LITERAL") {
        auto lit = l.IsDECIMAL_LITERAL("987");
        REQUIRE(lit != nullptr);
        REQUIRE(static_pointer_cast<DECIMAL_LITERAL>(lit)->value == 987);

        lit = l.IsDECIMAL_LITERAL("0");
        REQUIRE(lit == nullptr);

        lit = l.IsDECIMAL_LITERAL("a");
        REQUIRE(lit == nullptr);
    }

    SECTION("IsHEX_LITERAL") {
        auto lit = l.IsHEX_LITERAL("0xfa90");
        REQUIRE(lit != nullptr);
        REQUIRE(static_pointer_cast<HEX_LITERAL>(lit)->value == 0xfa90);

        lit = l.IsHEX_LITERAL("0xgg");
        REQUIRE(lit == nullptr);

        lit = l.IsHEX_LITERAL("fa90");
        REQUIRE(lit == nullptr);
    }

    SECTION("LexString") {
        auto tokens = l.LexString("MOVE $ACC, X+$A");
        REQUIRE(tokens.size() == 5);
        auto tokens_str = ToString(tokens);
        REQUIRE(tokens_str == "<MNEMONIC:mnemonic=MOVE><REGISTER:reg=ACC><COMMA><XPLUS><REGISTER:reg=A>");

        tokens = l.LexString("label: MOVE [X+$ACC], [Y-123] ; Comment\n");
        tokens_str = ToString(tokens);
        std::cout << tokens_str << std::endl;
        REQUIRE(tokens.size() == 13);
        REQUIRE(tokens_str == "<LABEL:label=label#6><MNEMONIC:mnemonic=MOVE><LEFT_BRACKET><XPLUS><REGISTER:reg=ACC><RIGHT_BRACKET><COMMA><LEFT_BRACKET><YMINUS><DECIMAL_LITERAL:value=123#3><RIGHT_BRACKET><COMMENT:comment= Comment><NEWLINE>");

        tokens = l.LexString(source);
        tokens_str = ToString(tokens);
        std::cout << tokens_str << std::endl;
        REQUIRE(tokens.size() != 0);
    }
}
