#ifndef QUNTOKEN_H
#define QUNTOKEN_H

/**
 * The interface to the quntoken library.
 */

#include "qxqueue.h"

QxQueue get_queue(const std::string& form, const std::string& mode, bool hyphen);

#endif
