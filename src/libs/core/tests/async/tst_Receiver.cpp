#include <gtest/gtest.h>

#include <ez/async/All.hpp>

using namespace ez;
using namespace ez::async;

TEST(Receiver, value)
{
    Receiver<std::string> receiver;

    ASSERT_FALSE(receiver.has_value());

    receiver.set_value("hello");

    ASSERT_TRUE(receiver.has_value());
}

TEST(Receiver, exception)
{
    Receiver<std::string> receiver;

    try {
        throw std::runtime_error("runtime error");
    }
    catch (...) {
        receiver.unhandled_exception();
    }

    ASSERT_FALSE(receiver.has_value());

    ASSERT_THROW(receiver.get(), std::runtime_error);
}

TEST(Receiver, empty)
{
    Receiver<std::string> receiver;

    ASSERT_FALSE(receiver.has_value());

    ASSERT_THROW(receiver.get(), ValueNotSet);
}


