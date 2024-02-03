#include <ez/rpc/messages.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ez::rpc {

RequestId make_request_id()
{
    return RequestId{boost::uuids::to_string(boost::uuids::random_generator()())};
}

}  // namespace ez::rpc
