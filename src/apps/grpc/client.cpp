#include <service.grpc.pb.h>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include <print>

int main()
{
    auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<ez::app::grpc::Echo::Stub> stub = ez::app::grpc::Echo::NewStub(channel);
    grpc::ClientContext context;

    {
        ez::app::grpc::Message request;
        request.set_content("hello");

        ez::app::grpc::Message reply;

        grpc::Status status = stub->identity(&context, request, &reply);

        std::println("Identity {}", reply.content());
    }

    {
        ez::app::grpc::Message reply;
        auto writer = stub->merge(&context, &reply);

        ez::app::grpc::Message request;
        request.set_content("1");
        writer->Write(request);

        request.set_content("2");
        writer->Write(request);

        request.set_content("3");
        writer->Write(request);

        writer->Finish();

        std::println("Merge result {}", reply.content());
    }

    {
        ez::app::grpc::Message request;
        request.set_content("123456789");

        auto reader = stub->split(&context, request);

        ez::app::grpc::Message reply;

        while (reader->Read(&reply)) { std::println("Split result {}", reply.content()); }
    }

    {
        auto stream = stub->reverse(&context);

        ez::app::grpc::Message request;
        request.set_content("hello");
        stream->Write(request);

        request.set_content("world");
        stream->Write(request);

        stream->Finish();

        ez::app::grpc::Message reply;

        while (stream->Read(&reply)) { std::println("Reverse result {}", reply.content()); }
    }
}
