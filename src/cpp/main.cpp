#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "output_conversions.h"
#include "prep_prep_lexer" // a *_lexer-eknek elobb kell lenniuk, mint a tobbi quex-esnek
#include "snt_snt_lexer"
#include "sntcorr_sntcorr_lexer"
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
        // kiiratas:
        /* std::string str(QUEX_CONVERTER_STRING(unicode, char)(token_prep_p->get_text())); */
        /* std::cout << convert_to_xml(str); */
        // tovabbadas:
        to_snt << QUEX_CONVERTER_STRING(unicode, char)(token_prep_p->get_text());
    }
    

    /* // sentences */
    snt::Token* token_snt_p = 0x0;
    snt::snt_lexer lexer_snt(&to_snt, "UTF8");
    std::stringstream to_sntcorr1; // output of snt modul, input of sntcorr modul
    for(lexer_snt.receive(&token_snt_p);
        token_snt_p->type_id() != SNT_TERMINATION;
        lexer_snt.receive(&token_snt_p))
    {
        // kiiratas:
        /* std::string str(QUEX_CONVERTER_STRING(unicode, char)(token_snt_p->get_text())); */
        /* std::cout << convert_to_xml(str); */
        // tovabbadas:
        to_sntcorr1 << QUEX_CONVERTER_STRING(unicode, char)(token_snt_p->get_text());
    }

    // sentence corrections 1.
    // Megj.: ketszer futtatjuk, mert az eredeti huntoken is ketszer futtatta,
    //      gondolom hogy az atfedo szabalyok mindegyike lefusson.
    sntcorr::Token* token_sntcorr1_p = 0x0;
    sntcorr::sntcorr_lexer lexer_sntcorr1(&to_sntcorr1, "UTF8");
    std::stringstream to_sntcorr2; // output of sntcorr1, input of sntcorr2
    for(lexer_sntcorr1.receive(&token_sntcorr1_p);
        token_sntcorr1_p->type_id() != SNTCORR_TERMINATION;
        lexer_sntcorr1.receive(&token_sntcorr1_p))
    {
        // kiiratas:
        /* std::string str(QUEX_CONVERTER_STRING(unicode, char)(token_sntcorr1_p->get_text())); */
        /* std::cout << convert_to_xml(str); */
        // tovabbadas:
        to_sntcorr2 << QUEX_CONVERTER_STRING(unicode, char)(token_sntcorr1_p->get_text());
    }

    // sentence corrections 2.
    sntcorr::Token* token_sntcorr2_p = 0x0;
    sntcorr::sntcorr_lexer lexer_sntcorr2(&to_sntcorr2, "UTF8");
    /* std::stringstream to_token; // output of sntcorr modul, input of token modul */
    for(lexer_sntcorr2.receive(&token_sntcorr2_p);
        token_sntcorr2_p->type_id() != SNTCORR_TERMINATION;
        lexer_sntcorr2.receive(&token_sntcorr2_p))
    {
        std::string str(QUEX_CONVERTER_STRING(unicode, char)(token_sntcorr2_p->get_text()));
        std::cout << convert_to_xml(str);
        /* to_token << QUEX_CONVERTER_STRING(unicode, char)(token_sntcorr2_p->get_text()); */
    }


    std::cout << std::endl;

    return 0;
}

/* Megj.: token kinyeresenek modjai: */
/* 1. típus + token, pretty-zve: token_p->get_string() */
/* 2. csak token, pretty-zve:    token_p->pretty_char_text() */
/* 3. nyers token: QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text()) */
/* (a pretty iszképeli a \n-, \r-, \t-ket, mast nem valtoztat) */

