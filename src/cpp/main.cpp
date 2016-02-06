#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <typeinfo>
#include "output_conversions.h"
#include "prep_prep_lexer" // a *_lexer-eknek elobb kell lenniuk, mint a tobbi quex-esnek
#include "snt_snt_lexer"
#include "sntcorr_sntcorr_lexer"
#include <quex/code_base/multi.i> // több quex modulhoz osszekapcsolasahoz
#include <quex/code_base/definitions> // QUEX_CONVERTER_STRING-hez


template<class INP, class OUT, class LEX, class TOK>
void using_module(INP input, OUT & output, QUEX_TYPE_TOKEN_ID termination) {
    TOK * token_p = 0x0;
    LEX lexer(input, "UTF8");
    for(lexer.receive(&token_p);
        token_p->type_id() != termination;
        lexer.receive(&token_p)) {
        output << QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text());
    }
}


int main(int argc, char** argv)
{
    // input file
    if (argc < 2)
    {
        std::cerr << "Missing file name!" << std::endl;
        exit(0);
    }
    FILE* fh = fopen(argv[1], "rb");

    std::stringstream to_snt; // output of prepocessing, input of snt processing
    using_module<FILE*, std::stringstream, prep::prep_lexer, prep::Token>(fh, to_snt, PREP_TERMINATION);
    std::stringstream to_sntcorr1; // output of snt modul, input of sntcorr modul
    using_module<std::stringstream*, std::stringstream, snt::snt_lexer, snt::Token>(&to_snt, to_sntcorr1, SNT_TERMINATION);
    std::stringstream to_sntcorr2; // output of sntcorr1, input of sntcorr2
    using_module<std::stringstream*, std::stringstream, sntcorr::sntcorr_lexer, sntcorr::Token>(&to_sntcorr1, to_sntcorr2, SNTCORR_TERMINATION);
    /* std::stringstream to_token; // output of sntcorr modul, input of token modul */
    using_module<std::stringstream*, std::ostream, sntcorr::sntcorr_lexer, sntcorr::Token>(&to_sntcorr2, std::cout, SNTCORR_TERMINATION);

    return 0;
}

/* Megj.: token kinyeresenek modjai: */
/* 1. típus + token, pretty-zve: token_p->get_string() */
/* 2. csak token, pretty-zve:    token_p->pretty_char_text() */
/* 3. nyers token: QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text()) */
/* (a pretty iszképeli a \n-, \r-, \t-ket, mast nem valtoztat) */

