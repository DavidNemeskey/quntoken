#include "qx_module_queue.h"
#include "qx_module.h"
#include <sstream>
#include <string>
#include <vector>


// constructor:
QxModuleQueue::QxModuleQueue(TYPE_VECTOR types, std::stringstream* fst_input_p, OUTPUT_TYPE out_type)
: types(types), modules(MODULE_VECTOR(types.size())), processed(false) {
    // empty queue, do nothing
    if(types.empty()) {
        return;
    }
    switch(out_type) {
        case XML:
            converter_p = new XmlConverter();
            break;
        case JSON:
            converter_p = new JsonConverter();
            break;
        default:
            // TODO!
            break;
    }
    // fill the modules vector
    std::stringstream* ss_p = nullptr;
    MODULE_VECTOR::iterator it = modules.begin();
    for (auto type : types) {
        it->set_type(type);
        // setup input pointers
        if(ss_p) {
            it->set_input_p(ss_p);
        }
        ss_p = it->get_output_p();
        ++it;
    }
    // setup input pointer of the first module
    modules.front().set_input_p(fst_input_p);
}

// destructor:
QxModuleQueue::~QxModuleQueue() {
    delete converter_p;
}

// private functions:
void QxModuleQueue::process() {
    for(MODULE_VECTOR::iterator it = modules.begin();
        it != modules.end();
        ++it) {
        it->using_module();
    }
    processed = true;
}

void QxModuleQueue::get_result(std::string& result) {
    if(!processed) {
        process();
    }
    result = modules.back().get_output_p()->str();
    converter_p->convert_tags(result);
}

void QxModuleQueue::print_result() {
    if(!processed) {
        process();
    }
    *converter_p << modules.back().get_output_p()->str();
}

