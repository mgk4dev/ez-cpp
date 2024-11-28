find_package(Boost COMPONENTS compute REQUIRED)
find_package(OpenCL REQUIRED)

add_executable(ez_app_cl
    main.cpp
)

target_link_libraries(ez_app_cl
    PRIVATE
#        ez_core
        Boost::boost
        OpenCL::OpenCL
)


