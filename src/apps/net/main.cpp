
#include "Tcp.hpp"

#include <ez/Print.hpp>

using namespace ez;

int main()
{
    net::IoContext io_context;

    net::TaskPool task_pool{io_context};

    net::tcp::EndPoint endpoint{net::Address::from_string("127.0.0.1"), 8080};

    task_pool << net::start_server(task_pool, endpoint);

    io_context.run();

    return 0;
}
