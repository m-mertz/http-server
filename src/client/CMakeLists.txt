find_cc_sources(./ CLIENT_SRC)

add_executable(client ${CLIENT_SRC})
target_link_libraries(client util)
