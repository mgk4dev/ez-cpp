
#include "Tcp.hpp"

#include <ez/ExecutionReport.hpp>

using namespace ez;

int main()
{
    net::IoContext io_context;
    net::Scope scope{io_context};
    net::tcp::EndPoint endpoint{net::Address::from_string("127.0.0.1"), 8080};

    scope << net::start_server(scope, endpoint);

    io_context.run();

    return 0;
}
