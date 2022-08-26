add_executable(array_custom "array_custom.c")
target_link_libraries(array_custom -lm zfp)

add_executable(main "main.c")
target_link_libraries(main -lm zfp)

add_executable(main_2 "main_2.c")
target_link_libraries(main_2 -lm zfp)