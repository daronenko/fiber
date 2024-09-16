project_log("CMake: Library Source")

get_filename_component(LIB_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(LIB_SOURCE_PATH "./fiber" ABSOLUTE)

file(GLOB_RECURSE LIB_CXX_SOURCES ${LIB_SOURCE_PATH}/*.cpp)
file(GLOB_RECURSE LIB_ASM_SOURCES ${LIB_SOURCE_PATH}/*.S)
file(GLOB_RECURSE LIB_HEADERS ${LIB_SOURCE_PATH}/*.hpp)

enable_language(ASM)

add_library(${LIB_TARGET} STATIC ${LIB_CXX_SOURCES} ${LIB_ASM_SOURCES} ${LIB_HEADERS})
target_include_directories(${LIB_TARGET} PUBLIC ${LIB_INCLUDE_PATH})
target_link_libraries(${LIB_TARGET} PUBLIC function2 sure wheels)
