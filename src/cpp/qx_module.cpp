#include "qx_module.h"
#include <iostream> // cerr
#include <cstdlib> // exit


// constructor:
QxModule::QxModule() : input_p(nullptr) { }

// destructor:
QxModule::~QxModule() { }

// private functions
void QxModule::set_type(MODULE_TYPE t) {
    type = t;
}

void QxModule::set_input_p(std::stringstream * inp_p) {
    input_p = inp_p;
}

std::stringstream* QxModule::get_output_p() {
    return &output;
}


void QxModule::using_module() {
    switch(type) {
        case PREP:
            module<prep::prep_lexer, prep::Token>(PREP_TERMINATION);
            break;
        case SNT:
            module<snt::snt_lexer, snt::Token>(SNT_TERMINATION);
            break;
        case SNTCORR:
            module<sntcorr::sntcorr_lexer, sntcorr::Token>(SNTCORR_TERMINATION);
            break;
        case TOKEN:
            module<token::token_lexer, token::Token>(TOKEN_TERMINATION);
            break;
        default:
            std::cerr << "Wrong module type!" << std::endl;
            exit(1);
    }
}

template <class LEXER, class TOKEN>
void QxModule::module(QUEX_TYPE_TOKEN_ID termination) {
    TOKEN* token_p = 0x0;
    LEXER lexer(input_p, "UTF8");
    for(lexer.receive(&token_p);
        token_p->type_id() != termination;
        lexer.receive(&token_p)) {
        output << token_p->get_text().c_str();
    }
}


/* Megj.: token kinyeresenek modjai: */
/* 1. típus + token, pretty-zve: token_p->get_string() */
/* 2. csak token, pretty-zve:    token_p->pretty_char_text() */
/* 3.a. nyers token: QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text()) */
/* 3.b. nyers token: token_p->get_text().c_str() */
/* (a pretty iszképeli a \n-, \r-, \t-ket, mast nem valtoztat) */

