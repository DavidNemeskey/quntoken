#include <exception>
#include <map>

#include "quntoken.h"


namespace {
std::map<std::string, MODULE_TYPE> formats =
    {{"xml", CONVXML}, {"json", CONVJSON}, {"vert", CONVVERT}};
}


QxQueue get_queue(const std::string& format,
                  const std::string& mode, bool hyphen) {
    std::vector<MODULE_TYPE> modules = {PREPROC, SNT, SNTCORR, SNTCORR};
    if (mode == "token") {
        modules.push_back(TOKEN);
    }
    if (hyphen) {
        modules.insert(modules.begin() + 1, HYPHEN);
    }
    try {
        if (format != "raw")
            modules.push_back(formats.at(format));
    } catch (std::out_of_range) {
        throw std::invalid_argument(
            "Wrong format. Valid formats: xml, json, vert, raw.");
    }
    return QxQueue(modules);
}
