find_cc_sources(./ SERVER_SRC)

add_executable(server ${SERVER_SRC})
target_link_libraries(server util)
