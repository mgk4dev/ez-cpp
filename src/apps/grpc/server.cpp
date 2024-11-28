#include <service.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include <print>

using ez::app::grpc::Message;

class EchoService final : public ez::app::grpc::Echo::Service {
public:
    ::grpc::Status identity(::grpc::ServerContext*,
                            const Message* request,
                            Message* response) override
    {
        response->set_content(request->content());

        return grpc::Status::OK;
    }

    ::grpc::Status merge(::grpc::ServerContext*,
                         ::grpc::ServerReader<Message>* reader,
                         Message* response) override
    {
        std::stringstream result;

        Message input;
        while (reader->Read(&input)) { result << input.content() << " "; }

        response->set_content(result.str());

        return grpc::Status::OK;
    }
    ::grpc::Status split(::grpc::ServerContext*,
                         const Message* request,
                         ::grpc::ServerWriter<Message>* writer) override
    {
        Message output;
        for (char c : request->content()) {
            output.set_content(std::string{c});
            writer->Write(output);
        }

        return grpc::Status::OK;
    }
    ::grpc::Status reverse(::grpc::ServerContext*,
                           ::grpc::ServerReaderWriter<Message, Message>* stream) override
    {
        Message request;

        while (stream->Read(&request)) {
            std::ranges::reverse(*request.mutable_content());
            stream->Write(request);
        }

        return grpc::Status::OK;
    }
};

int main()
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());

    EchoService service;
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
}
