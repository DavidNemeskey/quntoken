#include <cstdbool>
#include "quntoken.h"

/** Define C functions for the C++ class - as ctypes can only talk to C... */
extern "C" {
    /**
     * Wraps the constructor.
     *
     * \warning Note that C cannot handle exceptions, so we just return
     *          \c nullptr on error. Because of this, argument validation
     *          must happen on the Python side.
     */
    QunToken* QunToken_new(const char* format, const char* mode, bool hyphen) {
        try {
            return new QunToken(format, mode, hyphen);
        } catch (std::invalid_argument& ia) {
            return nullptr;
        }
    }
    /** Wraps QunToken::tokenize(std::string). Not thread-safe. */
    const char* QunToken_tokenize(QunToken* qt, const char* input) {
        /*
         * Static, so that it does not go out of scope. There's the GIL,
         * so thread safety is not an issue anyway.
         */
        static std::string result;
        // = doesn't work, even though it probably should with move semantics?
        result.assign(qt->tokenize(input));
        return result.c_str();
    }
}
