
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXTRA_CXX_FLAGS "")
set(EXTRA_CXX_FLAGS "")
set(EXTRA_CXX_FLAGS_RELEASE "")
set(EXTRA_CXX_FLAGS_DEBUG "")
set(EXTRA_EXE_LINKER_FLAGS "")
set(EXTRA_EXE_LINKER_FLAGS_RELEASE "")
set(EXTRA_EXE_LINKER_FLAGS_DEBUG "")


message("-- CMAKE_CXX_COMPILER : ${CMAKE_CXX_COMPILER_ID}")

# Do not export symbols by default on gcc, msvc or clang
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
set(COMPILER_IS_GCC ON)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
set(COMPILER_IS_MSVC ON)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
set(COMPILER_IS_CLANG ON)
endif()

# Overwrite default flags
if (UNIX) # Linux and Macos

    set (CMAKE_CXX_FLAGS                "")
    set (CMAKE_CXX_FLAGS_DEBUG          "-g -O0")
    set (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")

    if(ENABLE_SANITIZER)
      set (EXTRA_CXX_FLAGS                "${EXTRA_CXX_FLAGS} -fsanitize=${SANITIZER_OPTION}")
      set (EXTRA_EXE_LINKER_FLAGS         "${EXTRA_EXE_LINKER_FLAGS} -fsanitize=${SANITIZER_OPTION}")
      if(${SANITIZER_OPTION} STREQUAL "address")
        # needed to have better stack traces when using address sanitizer
        set (EXTRA_CXX_FLAGS             "${EXTRA_CXX_FLAGS}  -fno-omit-frame-pointer")
      endif()
    endif()
elseif (MSVC)

    if(NOT CMAKE_SIZEOF_VOID_P MATCHES "8")
        message(FATAL_ERROR "Windows tool chain requires MSVC x64 compiler")
    endif()

    add_definitions(-D_USE_MATH_DEFINES -D_SCL_SECURE_NO_WARNINGS)

    set (CMAKE_CXX_FLAGS                "/DWIN32 /D_WIN64 /GR /EHsc /D_UNICODE /DUNICODE")
    set (CMAKE_CXX_FLAGS_DEBUG          "/MDd /Zi /Od /RTC1")
    set (CMAKE_CXX_FLAGS_RELEASE        "/MD /O2 /DNDEBUG")

    set (CMAKE_EXE_LINKER_FLAGS          "/machine:x64")
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE  "")
    set (CMAKE_EXE_LINKER_FLAGS_DEBUG    "/debug")

    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} /bigobj") # to avoid number of sections exceeded object file format
endif()

if (COMPILER_IS_GCC)

    # Check compiler version
    if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${CMAKE_PROJECT_NAME} C++11 support requires g++ 4.7 or greater.")
    endif ()

    # Show more warnings/errors from compiler
    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Wall -Wextra -Wno-deprecated-declarations")
    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Wno-missing-field-initializers") # Ignore gmock warnings
    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Wno-invalid-offsetof") # Ignore "offsetof within non-standard-layout type {...} is undefined"

    # Turn all the warnings into errors
    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Werror")

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.8)

        # -Og is interesting to speed up the debug versions, but it affects to the debugging experience
        if (USE_DEBUG_OPTIMIZATIONS)
            set(EXTRA_CXX_FLAGS_DEBUG "${EXTRA_CXX_FLAGS_DEBUG} -Og")
        else()
            set(EXTRA_CXX_FLAGS_DEBUG "${EXTRA_CXX_FLAGS_DEBUG} -O0")
        endif()

        # Disable macro expansion, it is slow and uses up to 50% more memory
        # Use for crazy template developers
        set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -ftrack-macro-expansion=0")
        set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -ftemplate-backtrace-limit=0")

    else()
        set(EXTRA_CXX_FLAGS_DEBUG "${EXTRA_CXX_FLAGS_DEBUG} -O0")
    endif ()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6.0)
        # Disable -Wmisleading-indentation (enabled by -Wall) as it produces many false positives with macros
        set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Wno-misleading-indentation")
    endif()

elseif (COMPILER_IS_CLANG)

    if(NOT EXISTS ${CMAKE_CXX_COMPILER})
        message( FATAL_ERROR "Clang++ not found. " )
    endif()

    # Show more warnings/errors from compiler
    set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Wall -Wextra -Weverything -Wno-deprecated-declarations")

endif()


if (NOT(COMPILER_IS_MSVC OR COMPILER_IS_GCC OR COMPILER_IS_CLANG))

    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} compiler not supported in this project.")

endif()

# ==========================================================
# Add user supplied extra options (optimization, etc...)
# ==========================================================
set(EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS}" CACHE INTERNAL "Extra C++ compiler options")
set(EXTRA_CXX_FLAGS_RELEASE "${EXTRA_CXX_FLAGS_RELEASE}" CACHE INTERNAL "Extra compiler options for Release build")
set(EXTRA_CXX_FLAGS_DEBUG "${EXTRA_CXX_FLAGS_DEBUG}" CACHE INTERNAL "Extra compiler options for Debug build")
set(EXTRA_EXE_LINKER_FLAGS "${EXTRA_EXE_LINKER_FLAGS}" CACHE INTERNAL "Extra linker flags")
set(EXTRA_EXE_LINKER_FLAGS_RELEASE "${EXTRA_EXE_LINKER_FLAGS_RELEASE}" CACHE INTERNAL "Extra linker flags for Release build")
set(EXTRA_EXE_LINKER_FLAGS_DEBUG "${EXTRA_EXE_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "Extra linker flags for Debug build")

# Combine all "extra" options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_CXX_FLAGS_DEBUG}")

set(CMAKE_EXE_LINKER_FLAGS         "${CMAKE_EXE_LINKER_FLAGS} ${EXTRA_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${EXTRA_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${EXTRA_EXE_LINKER_FLAGS_DEBUG}")


# ==========================================================
# Colorize ninja output
# ==========================================================

if(COMPILER_IS_GCC)
    add_compile_options(-fdiagnostics-color=always)
elseif(COMPILER_IS_CLANG)
    add_compile_options(-fcolor-diagnostics)
endif()
