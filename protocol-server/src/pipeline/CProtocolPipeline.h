#include "../engine/interface/CProtocolSocket.h"
#include "../engine/socket/Socket.h"
#include "../engine/socket/SocketSelect.h"
#include "../utils/ProtocolHelper.h"
#include "../utils/logger/Logger.h"

extern "C" void *CProtocolPipeline(CProtocolSocket *ptr, void *lptr);