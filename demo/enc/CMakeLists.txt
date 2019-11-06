file(GLOB sources *.cpp)

set(name enc)
add_executable(${name} ${sources})

target_link_libraries(${name} prvc_enc ${COMMON_LIBS})
