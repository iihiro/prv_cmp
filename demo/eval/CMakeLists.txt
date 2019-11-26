file(GLOB sources *.cpp)

set(name eval)
add_executable(${name} ${sources})

target_link_libraries(${name} prvc_eval ${COMMON_LIBS})
