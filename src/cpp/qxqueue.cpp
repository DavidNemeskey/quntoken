#include <unistd.h>
#include <string>
#include <sstream>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "qxqueue.h"

/************* Macro stuff to generate lexer includes / namespaces ***********/

// List of the modules for boost preprocessor to work with
#define QX_MODULES (preproc) (hyphen) (snt) (sntcorr) (token) \
                   (convxml) (convjson) (convtsv)
// ... and the uppercase variants -- the PP cannot do capitalization 
#define QX_MODULES_UPPER (PREPROC) (HYPHEN) (SNT) (SNTCORR) (TOKEN) \
                         (CONVXML) (CONVJSON) (CONVTSV)

#define QX_MODULE_CASE(r, data, index, elem) \
            case BOOST_PP_SEQ_ELEM(index, QX_MODULES_UPPER): { \
                BOOST_PP_CAT(elem, BOOST_PP_EMPTY())::module(istreams[i], ostreams[i]); \
                break; \
            }

#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, 7, "qxmodules.h"))

#include BOOST_PP_ITERATE()

/***************************** End of macro stuff ***************************/

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

// public functions:
void QxQueue::run(std::istream& inp, std::ostream& out /* =std::cout */) {
    clear_streams();
    istreams.front() = &inp;
    ostreams.back() = &out;

    preproc::Lexer_ByteLoader_stream_new(&inp);

    for (int i = 0; i < types.size(); ++i) {
        switch(types[i]) {
            BOOST_PP_SEQ_FOR_EACH_I(QX_MODULE_CASE, _, QX_MODULES)
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

