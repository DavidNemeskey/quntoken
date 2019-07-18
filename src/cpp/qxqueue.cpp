#include <unistd.h>
#include <string>
#include <sstream>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include "qxqueue.h"

#include "preproc_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "hyphen_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "snt_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "sntcorr_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "token_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "convxml_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "convjson_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>
#include "convtsv_Lexer"
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>

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

#define MODULES (preproc) (hyphen) (snt) (sntcorr) (token) \
                (convxml) (convjson) (convtsv)

#define MODULE_NAMESPACE(r, data, elem) \
namespace elem { \
void module(std::istream* inp, std::ostream* out) { \
    Lexer_ByteLoader* loader = Lexer_ByteLoader_stream_new(inp); \
    Lexer_Converter* converter = Lexer_Converter_IConv_new("UTF8", nullptr); \
    Lexer* lexer = Lexer::from_ByteLoader(loader, converter); \
\
    typename Lexer::token_type* token_p = 0x0; \
    for (lexer->receive(&token_p); \
         token_p->type_id() != BOOST_PP_CAT(elem, _TERMINATION); \
         lexer->receive(&token_p)) { \
        *out << QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text()); \
    } \
    delete lexer; \
} \
}


BOOST_PP_SEQ_FOR_EACH(MODULE_NAMESPACE, _, MODULES)


// public functions:
void QxQueue::run(std::istream& inp, std::ostream& out /* =std::cout */) {
    clear_streams();
    istreams.front() = &inp;
    ostreams.back() = &out;

    preproc::Lexer_ByteLoader_stream_new(&inp);

    for (int i = 0; i < types.size(); ++i) {
        switch(types[i]) {
            case PREPROC:
                preproc::module(istreams[i], ostreams[i]);
                break;
            case HYPHEN:
                hyphen::module(istreams[i], ostreams[i]);
                break;
            case SNT:
                snt::module(istreams[i], ostreams[i]);
                break;
            case SNTCORR:
                sntcorr::module(istreams[i], ostreams[i]);
                break;
            case TOKEN:
                token::module(istreams[i], ostreams[i]);
                break;
            case CONVXML:
                convxml::module(istreams[i], ostreams[i]);
                break;
            case CONVJSON:
                convjson::module(istreams[i], ostreams[i]);
                break;
            case CONVVERT:
                convtsv::module(istreams[i], ostreams[i]);
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

