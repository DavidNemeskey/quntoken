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

QxQueue get_queue(const std::string& format, const std::string& mode, bool hyphen);

#endif
