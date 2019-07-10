#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

#include "quntoken.h"
// #include "qxqueue.h"


// globals:
const std::string HELP_STR = "Usage:\n"
                             "\tquntoken [OPTIONS] [-f FORMAT] FILE\n"
                             "Options:\n"
                             "\t-d\t\tRemove division of words at the end of the lines.\n"
                             "\t-m MODULE\tDefine target module. Valid targets: pre, snt, cor and tok. Default module: tok\n"
                             "\t-f FORMAT\tDefine output format. Valid formats: xml, json, vert and raw. Default format: vert.\n"
                             "\t-V\t\tDisplay version number and exit.\n"
                             "\t-h\t\tDisplay this help and exit";
const std::string VERSION  = "quntoken 1.2.0";


int main(int argc, char** argv) {

    // commandline arguments
    int c;
    int format_flag = 0;
    int hyphen_flag = 0;
    int module_flag = 0;
    std::string format;
    std::string mode;
    while ( (c = getopt(argc, argv, "hVdm:f:")) != -1 ) {
        switch (c) {
            case 'm':
                module_flag = 1;
                mode = optarg;
                break;
            case 'f':
                format_flag = 1;
                format = optarg;
                break;
            case 'd':
                hyphen_flag = 1;
                break;
            case 'h':
                std::cout << HELP_STR << std::endl;
                exit(0);
                break;
            case 'V':
                std::cout << VERSION << std::endl;
                exit(0);
                break;
            default:
                exit(1);
                break;
        }
    }

    // input file
    char* input_file;
    if (optind < argc) {
        input_file = argv[optind];
    } else {
        std::cerr << "Missing file name!" << std::endl;
        exit(1);
    }

    // modules
    if (!module_flag) {
        mode = "token";  // default
    }

    // output format
    if (!format_flag) {
        format = "vert";  // default
    }

    // hyphen
    bool hyphen = hyphen_flag ? true : false;

    // queue
    QxQueue q = get_queue(format, mode, hyphen);
    std::ifstream inp_fstream(input_file);
    q.run(inp_fstream);

    std::cout << std::endl;

    return 0;
}


