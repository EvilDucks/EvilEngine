
# GLAD
set(GLAD_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/glad/include)
set(GLAD_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/glad/src)
add_library(GLAD STATIC ${GLAD_SRC_DIR}/glad.c)
target_include_directories(GLAD PUBLIC ${GLAD_INC_DIR})

if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)
    # GLFW
    set(GLFW_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/glfw/include)
    add_library(GLFW INTERFACE)
    target_include_directories(GLFW INTERFACE ${GLFW_INC_DIR})
elseif (BUILD_TYPE EQUAL BUILD_TYPE_WIN)
    # WGLEXT
    set(WGLEXT_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/wglext/include)
    add_library(WGLEXT INTERFACE)
    target_include_directories(WGLEXT INTERFACE ${WGLEXT_INC_DIR})
endif ()

# GLM
set(GLM_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/glm/0.9.9.8)
add_library(GLM INTERFACE)
target_include_directories(GLM INTERFACE ${GLM_INC_DIR})

# STB
set(STB_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/stb)
add_library(STB INTERFACE)
target_include_directories(STB INTERFACE ${STB_INC_DIR})

# ASSIMP
set(ASSIMP_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/assimp/include)
add_library(ASSIMP INTERFACE)
target_include_directories(ASSIMP INTERFACE ${ASSIMP_INC_DIR})

# FreeType
set(FREETYPE_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/freetype/include)
add_library(FREETYPE INTERFACE)
target_include_directories(FREETYPE INTERFACE ${FREETYPE_INC_DIR})

# IMGUI
set(IMGUI_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
set(IMGUI_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
add_library(IMGUI STATIC
    ${IMGUI_SRC_DIR}/imgui.cpp
    ${IMGUI_SRC_DIR}/imgui_demo.cpp
    ${IMGUI_SRC_DIR}/imgui_draw.cpp
    #${IMGUI_SRC_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_SRC_DIR}/imgui_impl_win32.cpp
    ${IMGUI_SRC_DIR}/imgui_impl_opengl3.cpp
    ${IMGUI_SRC_DIR}/imgui_tables.cpp
    ${IMGUI_SRC_DIR}/imgui_widgets.cpp
    ${IMGUI_SRC_DIR}/imgui.cpp
)
target_include_directories(IMGUI PUBLIC ${IMGUI_SRC_DIR})
#target_link_libraries(IMGUI GLFW)
target_link_libraries(IMGUI
    Dwmapi
)

# SPDLOG
set(SPDLOG_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/spdlog/include)
add_library(SPDLOG INTERFACE)
target_include_directories(SPDLOG INTERFACE ${SPDLOG_INC_DIR})
