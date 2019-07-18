#ifndef QUNTOKEN_H
#define QUNTOKEN_H

/** Interface to the quntoken library. */

#include <iostream>
#include <memory>
#include <sstream>

#include "qxqueue.h"

/**
 * Class interface to the quntoken library.
 *
 * \warning This class is not thread-safe. Do not use in a multi-threaded
 *          context without synchronization.
 */
class QunToken {
  public:
    /**
     * Constructor.
     *
     * \param format: the output format.
     *                One of \c xml, \c json, \c tsv and \c raw.
     * \param mode: the modus operandi. One of \c sentence and \c token. Both
     *              perform sentence splitting; the latter tokenizes the text
     *              as well.
     * \param hyphen: whether to eliminate word breaks from the end of lines.
     */
    QunToken(const std::string& format, const std::string& mode, bool hyphen);

    /** Tokenizes \c input and returns a string. */
    std::string tokenize(const std::string& input);
    /** Tokenizes the contents of \c ins and writes them to \c outs. */
    inline void tokenize(std::istream& ins, std::ostream& outs=std::cout) {
        queue->run(ins, outs);
    }

  private:
    /** The queue used internally. */
    std::unique_ptr<QxQueue> queue;
    /** Used by <tt>tokenize(const std::string&)</tt>. */
    std::stringstream inss;
    /** Used by <tt>tokenize(const std::string&)</tt>. */
    std::stringstream outss;
};

#endif
