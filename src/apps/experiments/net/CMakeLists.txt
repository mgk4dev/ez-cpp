find_package(Boost COMPONENTS asio beast REQUIRED)

add_executable(net
    Http.hpp
    Operations.hpp
    Types.hpp
    Websockets.hpp
    main.cpp
)

target_link_libraries(net
    PRIVATE
        ez_core
        Boost::boost
)


