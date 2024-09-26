#pragma once

#include <ez/flow/engine.hpp>
#include <ez/flow/extensions.hpp>

#include <ez/async/Executor.hpp>
#include <ez/async/Schedule.hpp>

#include <boost/process.hpp>

#include <iostream>

namespace ez::flow::ext {
inline std::string to_string(boost::json::value value)
{
    return boost::json::value_to<std::string>(value);
}

JsonObject run_action(JsonObject action)
{
    try {
        std::string command = boost::json::value_to<std::string>(action["command"]);

        std::string args;

        if (action.contains("args")) { args = boost::json::value_to<std::string>(action["args"]); }

        int rc = boost::process::system(command, args);

        JsonObject response;
        response["return_code"] = rc;
        return response;
    }
    catch (const std::exception& error) {
        JsonObject response;
        response["error"] = error.what();
        return response;
    }
}

JsonObject run_campaign(JsonObject campaign)
{
    try {
        std::string question = to_string(campaign["question"]);

        std::cout << question << std::endl;

        uint index = 1;
        for (const auto& option : campaign.at("options").as_array()) {
            std::cout << "(" << index++ << ") " << to_string(option) << std::endl;
        }

        std::cout << ">> " << std::flush;

        std::cin.clear();
        std::cin >> index;

        JsonObject response;
        bool ok = (index <= campaign.at("options").as_array().size()) && (index > 0);
        response["success"] = ok;
        response["reponse"] = JsonObject{{"index", index}};
        return response;
    }
    catch (const std::exception& error) {
        JsonObject response;
        response["error"] = error.what();
        return response;
    }
}

JsonObject get_device_info(JsonObject)
{
    JsonObject response;
    response["name"] = "my device";
    response["os"] = "macos";
    response["cpu_count"] = 8;
    response["ram"] = std::int64_t(16l * 1024 * 1024 * 1024);

    return response;
}

JsonObject make_http_request(JsonObject input)
{
    try {
        auto method = to_string(input["method"]);
        auto resource = to_string(input["resource"]);
        auto payload = to_string(input["payload"]);

        int rc = boost::process::system("curl", std::format("--request {}", method),
                                        std::format("--url {}", resource),

                                        std::format("--data '{}'", payload));
        JsonObject response;
        response["success"] = rc;
        return response;
    }

    catch (const std::exception& error) {
        JsonObject response;
        response["error"] = error.what();
        return response;
    }
}

void setup_engine(Engine& engine, async::IoContext& io_context, async::ThreadPool& thread_pool)
{
    auto make_task = [&](auto impl) {
        return [&io_context, &thread_pool, impl](JsonObject request) -> async::Task<JsonObject> {
            co_await async::schedule_on(thread_pool);
            auto reply = impl(request);
            co_await async::schedule_on(io_context);
            co_return reply;
        };
    };

    engine.set_action_delegate(make_task(run_action));
    engine.set_campaign_delegate(make_task(run_campaign));
    engine.set_device_info_delegate(make_task(get_device_info));
    engine.set_http_request_delegate(make_task(make_http_request));
}

}  // namespace ez::flow::ext
