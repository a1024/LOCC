set(CMAKE_C_FLAGS "-g -mavx2 -mlzcnt")
add_executable(locc
	locc.c
	util.c
)
