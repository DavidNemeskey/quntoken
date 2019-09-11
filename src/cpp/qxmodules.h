/**
 * To be included into qxqueue.cpp by boost preprocessor, once per module.
 * Hence, no include guards.
 *
 * Does two things for each module:
 * 1. includes the header (along with the Converter headers)
 * 2. defines the module function used in qxqueue.cpp.
 *
 * The macros QX_MODULES and QX_MODULES_UPPER are defined in qxqueue.cpp
 * prior to including this file. The same goes for the BOOST_PP macros:
 * the includes are in the cpp file.
 */

#define QX_MODULE BOOST_PP_SEQ_ELEM(BOOST_PP_ITERATION(), QX_MODULES)
#define QX_INC_FILE_NAME BOOST_PP_STRINGIZE(BOOST_PP_CAT(QX_MODULE, _Lexer))
#include QX_INC_FILE_NAME

#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv>
#include <quex/code_base/buffer/lexatoms/converter/iconv/Converter_IConv.i>

namespace QX_MODULE {
void module(std::istream* inp, std::ostream* out) {
    Lexer_ByteLoader* loader = Lexer_ByteLoader_stream_new(inp);
    Lexer_Converter* converter = Lexer_Converter_IConv_new("UTF8", nullptr);
    Lexer* lexer = Lexer::from_ByteLoader(loader, converter);

    typename Lexer::token_type* token_p = 0x0;
    for (lexer->receive(&token_p);
         token_p->type_id() != BOOST_PP_CAT(QX_MODULE, _TERMINATION);
         lexer->receive(&token_p)) {
        *out << QUEX_CONVERTER_STRING(unicode, char)(token_p->get_text());
    }
    delete lexer;
}
}

#undef QX_INC_FILE_NAME
#undef QX_MODULE
