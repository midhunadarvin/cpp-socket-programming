#include "engine/interface/CProtocolSocket.h"
#include "handler/DefaultProtocolHandler.h"
#include "pipeline/Pipeline.h"
#include "pipeline/CProtocolPipeline.h"
#include "utils/logger/Logger.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    /* code */
    std::cout << "Hello world" << std::endl;
    std::string protocolName = "ChistaDATA";


    // Setup up Protocol Socket 
    CProtocolSocket* socket = new CProtocolSocket(8080);
     // Setting up Protocol Pipeline
    PipelineFunction<CProtocolSocket> pipelineFunction = CProtocolPipeline;
    if (!(*socket).SetPipeline(pipelineFunction)) {
        LOG_ERROR("Failed to set " + protocolName + " Pipeline ..!");
        return -2;
    }
    auto *protocolHandler = new DefaultProtocolHandler();
    if (protocolHandler) {
        if (!(*socket).SetHandler(protocolHandler)) {
            LOG_ERROR("Failed to set " + protocolName + " Handler ..!");
            return -2;
        }
    }

    if (!(*socket).Start()) {
        LOG_ERROR("Failed To Start " + protocolName + " Proxy Server ..!");
        return -3;
    }
    
    pause();
    return 0;
}
