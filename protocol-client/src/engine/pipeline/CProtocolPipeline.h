#include "../interface/CProtocolSocket.h"
#include "../socket/Socket.h"
#include "../socket/SocketSelect.h"
#include "../../utils/ProtocolHelper.h"
#include "../../utils/logger/Logger.h"

extern "C" void *CProtocolPipeline(CProtocolSocket *ptr, void *lptr);