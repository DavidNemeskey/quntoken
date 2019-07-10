#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "qxqueue.h"

#include "preproc_Lexer"
#include "hyphen_Lexer"
#include "snt_Lexer"
#include "sntcorr_Lexer"
#include "token_Lexer"
#include "convxml_Lexer"
#include "convjson_Lexer"
#include "convvert_Lexer"

#include <quex/code_base/multi.i>
#include <quex/code_base/definitions>



// ctor:
QxQueue::QxQueue(TYPE_VECTOR types) :
        types(types),
        istreams(ISTREAM_VECTOR(types.size(), nullptr)),
        ostreams(OSTREAM_VECTOR(types.size(), nullptr)) {
    create_streams();
}


// dtor:
QxQueue::~QxQueue() {
    delete_streams();
}


// private functions:
void QxQueue::create_streams() {
    for (size_t i = 0; i < ostreams.size() - 1; ++i) {
        std::stringstream* ss = new std::stringstream;
        istreams[i + 1] = ss;
        ostreams[i] = ss;
    }
}


void QxQueue::delete_streams() {
    for (size_t i = 0; i < ostreams.size() - 1; ++i) {
        delete ostreams[i];
        istreams[i + 1] = nullptr;
        ostreams[i] = nullptr;
    }
}


void QxQueue::clear_streams() {
    for (size_t i = 0; i < ostreams.size() - 1; ++i) {
        dynamic_cast<std::stringstream*>(ostreams[i])->str("");
        ostreams[i]->clear();
    }
}


// auxiliary function, not a QxQueue member
template <class LEXER, class TOKEN, QUEX_TYPE_TOKEN_ID termination>
void module(std::istream* inp, std::ostream* out) {
    TOKEN* token_p = 0x0;
    LEXER lexer(inp, "UTF8");
    for (lexer.receive(&token_p);
         token_p->type_id() != termination;
         lexer.receive(&token_p)) {
        *out << QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text());
    }
}


// public functions:
void QxQueue::run(std::istream& inp, std::ostream& out /* =std::cout */) {
    clear_streams();
    istreams.front() = &inp;
    ostreams.back() = &out;

    for (int i = 0; i < types.size(); ++i) {
        switch(types[i]) {
            case PREPROC:
                module<preproc::Lexer, preproc::Token, preproc_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case HYPHEN:
                module<hyphen::Lexer, hyphen::Token, hyphen_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case SNT:
                module<snt::Lexer, snt::Token, snt_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case SNTCORR:
                module<sntcorr::Lexer, sntcorr::Token, sntcorr_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case TOKEN:
                module<token::Lexer, token::Token, token_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case CONVXML:
                module<convxml::Lexer, convxml::Token, convxml_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case CONVJSON:
                module<convjson::Lexer, convjson::Token, convjson_TERMINATION>(istreams[i], ostreams[i]);
                break;
            case CONVVERT:
                module<convvert::Lexer, convvert::Token, convvert_TERMINATION>(istreams[i], ostreams[i]);
                break;
            default:
                std::cerr << "Wrong module type!" << std::endl;
                exit(1);
        }
    }

    istreams.front() = nullptr;
    ostreams.back() = nullptr;
}



/* Megj.: token kinyeresenek modjai: */
/* 1. típus + token, pretty-zve: token_p->get_string() */
/* 2. csak token, pretty-zve:    token_p->pretty_char_text() */
/* 3. nyers token: QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text()) */
/* (a pretty iszképeli a \n-, \r-, \t-ket, mast nem valtoztat) */
// TODO: wstring- / wstringstream-re atirni, hogy a kevesebb konverzioval
// idot nyerjunk! Eleg lenne a kiiras elott konvertalni.

