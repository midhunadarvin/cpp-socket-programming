#include <iostream>
#include <sstream> // provides templates and types that enable interoperation between stream buffers and string objects
#include "../test/tests.h"
#include "proxy.h"

int proxy_port;
std::string server_addr;
int server_port;

int main(int argc, char *argv[])
{


    // Run the tests
    // runTests();

    if (argc < 4)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "  proxy <port proxy> <addr server> <port server>" << std::endl;
        std::cout << std::endl;
        std::cout << "  This proxy will then listen on <port proxy> and whenever it receives" << std::endl;
        std::cout << "  a connection, relays the traffic to the <port server> of <addr server>." << std::endl;
        std::cout << "  This makes it ideal to see what an SMTP Client exchanges with a SMTP Server," << std::endl;
        std::cout << "  or equally what a NNTP client exchanges with an NNTP Server." << std::endl
                  << std::endl;
        return -1;
    }

    std::stringstream s;

    s << argv[1];
    s >> proxy_port;
    s.clear();

    server_addr = argv[2];

    s << argv[3];
    s >> server_port;

    Proxy proxy(proxy_port, server_addr, server_port);
    proxy.StartListeningThread();
}