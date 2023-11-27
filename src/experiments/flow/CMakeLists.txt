find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(flow
    extensions.hpp
    main.cpp
)

target_link_libraries(flow
    PRIVATE
        ez_flow
        Boost::boost
        Boost::program_options
)


