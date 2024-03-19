# Add header files
file(GLOB_RECURSE HEADER_FILES
	${PROJECT_SOURCE_DIR}/inc/*.h
	${PROJECT_SOURCE_DIR}/inc/*.hpp
)

add_compile_definitions(PLATFORM=${BUILD_TYPE})
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)


# Define the executable
if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)
	add_executable(${PROJECT_NAME} ${HEADER_FILES} src/agn_main.cpp)
elseif (BUILD_TYPE EQUAL BUILD_TYPE_WIN)
	#set (WIN32_RESOURCES ${CMAKE_SOURCE_DIR}/res/resource.rc)
	add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} src/win_main.cpp ${WIN_RESOURCES})
endif ()


target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(${PROJECT_NAME}
	kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32 opengl32
)

# Add SubProjects
target_link_libraries(${PROJECT_NAME} GLAD)
target_link_libraries(${PROJECT_NAME} GLM)
target_link_libraries(${PROJECT_NAME} STB)
target_link_libraries(${PROJECT_NAME} ASSIMP)
#target_link_libraries(${PROJECT_NAME} FreeType)

if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)
	target_link_libraries(${PROJECT_NAME} GLFW)

	# This Creates a command that will trigger POST_BUILD
	#  it will place subproject .dlls inside project build directory.
	add_custom_command ( TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:GLFW> $<TARGET_FILE_DIR:${PROJECT_NAME}>
	)

elseif (BUILD_TYPE EQUAL BUILD_TYPE_WIN)
	target_link_libraries(${PROJECT_NAME} WGLEXT)
endif ()

# DEBUG SubProjects
target_link_libraries(${PROJECT_NAME} SPDLOG)
target_link_libraries(${PROJECT_NAME} IMGUI)

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E create_symlink 
	${CMAKE_SOURCE_DIR}/project/res
	${CMAKE_CURRENT_BINARY_DIR}/res
)
