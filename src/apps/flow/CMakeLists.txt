find_package(Boost COMPONENTS filesystem program_options REQUIRED)

add_executable(flow
    Extensions.hpp
    main.cpp
)

target_link_libraries(flow
    PRIVATE
        ez_flow
        Boost::boost
        Boost::program_options
        Boost::filesystem
)


