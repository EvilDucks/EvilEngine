# Add header files
file(GLOB_RECURSE HEADER_FILES
    ${PROJECT_SOURCE_DIR}/inc/*.h
    ${PROJECT_SOURCE_DIR}/inc/*.hpp
)

# To EXCLUDE WIN PLATFORM INCLUDES
# does not work!
#file(GLOB_RECURSE HEADER_FILES
#    ${PROJECT_SOURCE_DIR}/inc/platform*.h ${PROJECT_SOURCE_DIR}/inc/*.hpp
#)
#list(FILTER HEADER_FILES EXCLUDE REGEX ".*window\\.hpp$")

# Define the executable

if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)
    add_executable(${PROJECT_NAME} ${HEADER_FILES} src/agn_main.cpp)
elseif (BUILD_TYPE EQUAL BUILD_TYPE_WIN)
    add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} src/win_main.cpp ${WIN_RESOURCES})
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(${PROJECT_NAME}
        kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32 opengl32
)

target_link_libraries(${PROJECT_NAME} GLAD)
target_link_libraries(${PROJECT_NAME} GLM)
target_link_libraries(${PROJECT_NAME} STB)
target_link_libraries(${PROJECT_NAME} ASSIMP)
#target_link_libraries(${PROJECT_NAME} FreeType)

if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)
    target_link_libraries(${PROJECT_NAME} GLFW)
elseif (BUILD_TYPE EQUAL BUILD_TYPE_WIN)
    target_link_libraries(${PROJECT_NAME} WGLEXT)
endif ()

# DEBUG
target_link_libraries(${PROJECT_NAME} SPDLOG)
target_link_libraries(${PROJECT_NAME} IMGUI)
