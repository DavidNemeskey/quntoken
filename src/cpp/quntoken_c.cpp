#include <cstdbool>
#include <string>
#include "quntoken.h"

/** Define C functions for the C++ class - as ctypes can only talk to C... */
extern "C" {
    /**
     * A string storing the cause of the exceptions caught by the \c QunToken_
     * functions. This errno-like functionality is needed as C doesn't handle
     * exceptions, and hence ctypes doesn't either.
     */
    thread_local std::string foo;

    /**
     * Wraps the constructor.
     *
     * \warning Note that C cannot handle exceptions, so we just return
     *          \c nullptr on error. Because of this, argument validation
     *          must happen on the Python side.
     */
    QunToken* QunToken_new(const char* format, const char* mode, bool hyphen) {
        foo = "";
        try {
            return new QunToken(format, mode, hyphen);
        } catch (std::invalid_argument& ia) {
            foo = ia.what();
            return nullptr;
        }
    }

    /** Wraps QunToken::tokenize(std::string). Not thread-safe. */
    const char* QunToken_tokenize(QunToken* qt, const char* input) {
        foo = "";
        /*
         * Static, so that it does not go out of scope. There's the GIL,
         * so thread safety is not an issue anyway.
         */
        static std::string result;
        // = doesn't work, even though it probably should with move semantics?
        try {
            result.assign(qt->tokenize(input));
        } catch (std::runtime_error& re) {
            foo = re.what();
            return nullptr;
        }
        return result.c_str();
    }

    /**
     * Returns the error message of the last exception caught by the other
     * \c QunToken_ functions, if any. A successful call to those functions will
     * erase the error message.
     */
    const char* QunToken_explain() {
        return foo.c_str();
    }
}
