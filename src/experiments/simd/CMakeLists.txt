
find_package(eve REQUIRED)
find_package(benchmark REQUIRED)

add_executable(ez_experiments_simd
    main.cpp
)

target_link_libraries(ez_experiments_simd
    PRIVATE
        eve::eve
        benchmark::benchmark_main
)

