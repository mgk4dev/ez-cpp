# Find *.hpp and *.cpp files and creates a static library.
# Public headers expected to be in the in the ${source_folder}/include folder
function (add_static_library target_name source_folder)
    message("++++ Adding static library '${target_name}' from source folder '${source_folder}'")
    file(GLOB_RECURSE header_files_ RELATIVE ${source_folder} *.hpp)
    file(GLOB_RECURSE source_files_ RELATIVE ${source_folder} *.cpp)
    message("++++ Header files '${header_files_}'")
    message("++++ Source files '${source_files_}'")

    add_library(
        ${target_name} 
        STATIC 
            ${header_files_}
            ${source_files_}
        )   

    target_include_directories(${target_name} PUBLIC ${source_folder}/include)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
    target_compile_features(${target_name} PUBLIC cxx_std_23)
endfunction()



function (add_modules_libray target_name)
    add_library(${target_name}) 
    target_sources(
        ${target_name}
        PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES FILES
            ${ARGN}
    )
endfunction()