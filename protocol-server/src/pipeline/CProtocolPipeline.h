#include "../engine/interface/CProtocolSocket.h"
#include "../engine/socket/Socket.h"
#include "../engine/socket/SocketSelect.h"
#include "../utils/ProtocolHelper.h"

extern "C" void *CProtocolPipeline(CProtocolSocket *ptr, void *lptr);