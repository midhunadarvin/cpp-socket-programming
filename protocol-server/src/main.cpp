#include "engine/interface/CProtocolSocket.h"
#include "handler/DefaultProtocolHandler.h"
#include "handler/SchedulerHandler.h"
#include "pipeline/CProtocolPipeline.h"
#include "pipeline/Pipeline.h"
#include "utils/logger/Logger.h"
#include <unistd.h>

int setupDefaultPipeline() {
  std::string protocolName = "default";

  // Setup up Protocol Socket
  CProtocolSocket *socket = new CProtocolSocket(8080);
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

	return 0;
}

int setupSchedulerPipeline() {
  std::string protocolName = "scheduler";

  // Setup up Protocol Socket
  CProtocolSocket *socket = new CProtocolSocket(8081);
  // Setting up Protocol Pipeline
  PipelineFunction<CProtocolSocket> pipelineFunction = CProtocolPipeline;
  if (!(*socket).SetPipeline(pipelineFunction)) {
    LOG_ERROR("Failed to set " + protocolName + " Pipeline ..!");
    return -2;
  }
  auto *protocolHandler = new SchedulerHandler();
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

	return 0;
}

int main(int argc, char const *argv[]) {

  // setup default pipeline
  setupDefaultPipeline();

  // setup scheduler pipeline
  setupSchedulerPipeline();

  pause();
  return 0;
}
