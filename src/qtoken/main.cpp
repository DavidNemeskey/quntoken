#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "prep_prep_lexer" // ennek előbb kell lennie, mint a többi quex-esnek
#include "snt_snt_lexer" // ennek előbb kell lennie, mint a többi quex-esnek
#include <quex/code_base/multi.i> // a több quex modulhoz
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
    std::stringstream tmp; // output of prepocessing, input of snt processing
    for(lexer_prep.receive(&token_prep_p);
        token_prep_p->type_id() != PREP_TERMINATION;
        lexer_prep.receive(&token_prep_p))
    {
        // típus + token (pretty-zve):
        /* std::cout << token_p->get_string() << std::endl; */
        // csak token, de a pretty iszképeli a \n-, \r-, \t-ket:
        /* std::cout << token_prep_p->pretty_char_text() << std::endl; */
        // csak token, nyersen:
        tmp << QUEX_CONVERTER_STRING(unicode, char)(token_prep_p->get_text());
    }
    

    // sentence processing
    snt::Token* token_snt_p = 0x0;
    snt::snt_lexer lexer_snt(&tmp, "UTF8");
    for(lexer_snt.receive(&token_snt_p);
        token_snt_p->type_id() != SNT_TERMINATION;
        lexer_snt.receive(&token_snt_p))
    {
        /* std::cout << token_snt_p->pretty_char_text() << std::endl; */
        std::cout << QUEX_CONVERTER_STRING(unicode, char)(token_snt_p->get_text()) << std::endl;
    }

    return 0;
}

