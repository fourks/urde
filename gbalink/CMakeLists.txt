include_directories(. ./../Runtime ${ATHENA_INCLUDE_DIR} ${JBUS_INCLUDE_DIR})
add_executable(gbalink main.cpp)
target_link_libraries(gbalink jbus)
