add_subdirectory(dependencies)

add_subdirectory(component)
add_subdirectory(crypto)
add_subdirectory(cvarcmd)
add_subdirectory(filesystem)
add_subdirectory(game)
add_subdirectory(input)
add_subdirectory(render)

add_sources(
	utf8.c
	common.cpp
	log.cpp
	main.cpp
	perfclock.cpp
	sys.cpp
)

# precompiled header
set(PCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/pch.h" PARENT_SCOPE)
