#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "prep_prep_lexer" // a *_lexer-eknek elobb kell lenniuk, mint a tobbi quex-esnek
#include "snt_snt_lexer"
#include "abbrev_abbrev_lexer"
#include <quex/code_base/multi.i> // több quex modulhoz osszekapcsolasahoz
#include <quex/code_base/definitions> // QUEX_CONVERTER_STRING-hez


int main(int argc, char** argv)
{
    // input file
    if (argc < 2)
    {
        std::cerr << "Missing file name!" << std::endl;
        exit(0);
    }
    FILE* fh = fopen(argv[1], "rb");

    // preprocessing
    prep::Token* token_prep_p = 0x0;
    prep::prep_lexer lexer_prep(fh, "UTF8");
    std::stringstream to_snt; // output of prepocessing, input of snt processing
    for(lexer_prep.receive(&token_prep_p);
        token_prep_p->type_id() != PREP_TERMINATION;
        lexer_prep.receive(&token_prep_p))
    {
        // típus + token (pretty-zve):
        /* std::cout << token_p->get_string() << std::endl; */
        // csak token, de a pretty iszképeli a \n-, \r-, \t-ket:
        /* std::cout << token_prep_p->pretty_char_text() << std::endl; */
        // csak token, nyersen:
        to_snt << QUEX_CONVERTER_STRING(unicode, char)(token_prep_p->get_text());
    }
    

    // sentences
    snt::Token* token_snt_p = 0x0;
    snt::snt_lexer lexer_snt(&to_snt, "UTF8");
    std::stringstream to_abbrev; // output of snt modul, input of abbrev modul
    for(lexer_snt.receive(&token_snt_p);
        token_snt_p->type_id() != SNT_TERMINATION;
        lexer_snt.receive(&token_snt_p))
    {
        /* std::cout << QUEX_CONVERTER_STRING(unicode, char)(token_snt_p->get_text()) << std::endl; */
        to_abbrev << QUEX_CONVERTER_STRING(unicode, char)(token_snt_p->get_text());
    }

    // abbreviations
    abbrev::Token* token_abbrev_p = 0x0;
    abbrev::abbrev_lexer lexer_abbrev(&to_abbrev, "UTF8");
    // std::stringstream to_token; // output of abbrev modul, input of token modul
    for(lexer_abbrev.receive(&token_abbrev_p);
        token_abbrev_p->type_id() != ABBREV_TERMINATION;
        lexer_abbrev.receive(&token_abbrev_p))
    {
        std::cout << QUEX_CONVERTER_STRING(unicode, char)(token_abbrev_p->get_text());
        /* to_token << QUEX_CONVERTER_STRING(unicode, char)(token_abbrev_p->get_text()); */
    }

    std::cout << std::endl;

    return 0;
}

