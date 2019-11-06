file(GLOB sources *.cpp)

set(name dec)
add_executable(${name} ${sources})

target_link_libraries(${name} prvc_dec ${COMMON_LIBS})
