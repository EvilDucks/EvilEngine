include(CPM)

# GLAD
set(GLAD_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/glad/include)
set(GLAD_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/glad/src)
add_library(GLAD STATIC ${GLAD_SRC_DIR}/glad.c)
target_include_directories(GLAD PUBLIC ${GLAD_INC_DIR})

if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)

    # GLFW
    set(GLFW_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/glfw/include)
	add_library(GLFW SHARED IMPORTED GLOBAL)
	set_property(TARGET GLFW PROPERTY IMPORTED_IMPLIB_RELEASE ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/glfw3.lib)
	set_property(TARGET GLFW PROPERTY IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_LIST_DIR}/glfw/bin/glfw3.dll)
	set_property(TARGET GLFW PROPERTY IMPORTED_IMPLIB_DEBUG ${CMAKE_CURRENT_LIST_DIR}/glfw/lib/glfw3.lib)
	set_property(TARGET GLFW PROPERTY IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/glfw/bin/glfw3.dll)
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
#set(ASSIMP_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/assimp/include)
#add_library(ASSIMP INTERFACE)
#target_include_directories(ASSIMP INTERFACE ${ASSIMP_INC_DIR})

# FreeType
set(FREETYPE_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/freetype/include)
add_library(FREETYPE SHARED IMPORTED GLOBAL)
set_property(TARGET FREETYPE PROPERTY IMPORTED_IMPLIB_RELEASE ${CMAKE_CURRENT_LIST_DIR}/freetype/lib/release/freetype.lib)
set_property(TARGET FREETYPE PROPERTY IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_LIST_DIR}/freetype/lib/release/freetype.dll)
set_property(TARGET FREETYPE PROPERTY IMPORTED_IMPLIB_DEBUG ${CMAKE_CURRENT_LIST_DIR}/freetype/lib/debug/freetype.lib)
set_property(TARGET FREETYPE PROPERTY IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/freetype/lib/debug/freetype.dll)
target_include_directories(FREETYPE INTERFACE ${FREETYPE_INC_DIR})

# CGLTF
set(CGLTF_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/cgltf)
add_library(CGLTF INTERFACE)
target_include_directories(CGLTF INTERFACE ${CGLTF_INC_DIR})

# TINY_OBJ_LOADER
set(TINY_OBJ_LOADER_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/tiny_obj_loader)
add_library(TINY_OBJ_LOADER INTERFACE)
target_include_directories(TINY_OBJ_LOADER INTERFACE ${TINY_OBJ_LOADER_INC_DIR})

# NLOHMANN_JSON
set(NLOHMANN_JSON_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/nlohmann-json)
add_library(NLOHMANN_JSON INTERFACE)
target_include_directories(NLOHMANN_JSON INTERFACE ${NLOHMANN_JSON_INC_DIR})

# EFFOLKRONIUM_RANDOM
set(EFFOLKRONIUM_RANDOM_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/effolkronium-random)
add_library(EFFOLKRONIUM_RANDOM INTERFACE)
target_include_directories(EFFOLKRONIUM_RANDOM INTERFACE ${EFFOLKRONIUM_RANDOM_INC_DIR})

#CPMAddPackage("gh:kcat/openal-soft#1.23.1")
#set_target_properties(OpenAL PROPERTIES FOLDER "dependencies")

# OPENAL
set(OPENAL_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/openal/include)
add_library(OPENAL SHARED IMPORTED GLOBAL)
set_property(TARGET OPENAL PROPERTY IMPORTED_IMPLIB_RELEASE ${CMAKE_CURRENT_LIST_DIR}/openal/lib/release/OpenAL32.lib)
set_property(TARGET OPENAL PROPERTY IMPORTED_LOCATION_RELEASE ${CMAKE_CURRENT_LIST_DIR}/openal/bin/release/OpenAL32.dll)
set_property(TARGET OPENAL PROPERTY IMPORTED_IMPLIB_DEBUG ${CMAKE_CURRENT_LIST_DIR}/openal/lib/debug/OpenAL32.lib)
set_property(TARGET OPENAL PROPERTY IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/openal/bin/debug/OpenAL32.dll)
target_include_directories(OPENAL INTERFACE ${OPENAL_INC_DIR})

# IMGUI
set(IMGUI_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
set(IMGUI_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
if (BUILD_TYPE EQUAL BUILD_TYPE_AGN)
    add_library(IMGUI STATIC
		${IMGUI_SRC_DIR}/imgui.cpp
		${IMGUI_SRC_DIR}/imgui_demo.cpp
		${IMGUI_SRC_DIR}/imgui_draw.cpp
		${IMGUI_SRC_DIR}/imgui_impl_glfw.cpp
		${IMGUI_SRC_DIR}/imgui_impl_opengl3.cpp
		${IMGUI_SRC_DIR}/imgui_tables.cpp
		${IMGUI_SRC_DIR}/imgui_widgets.cpp
		${IMGUI_SRC_DIR}/imgui.cpp
    )
    target_link_libraries(IMGUI GLFW)
elseif (BUILD_TYPE EQUAL BUILD_TYPE_WIN)
    add_library(IMGUI STATIC
		${IMGUI_SRC_DIR}/imgui.cpp
		${IMGUI_SRC_DIR}/imgui_demo.cpp
		${IMGUI_SRC_DIR}/imgui_draw.cpp
		${IMGUI_SRC_DIR}/imgui_impl_win32.cpp
		${IMGUI_SRC_DIR}/imgui_impl_opengl3.cpp
		${IMGUI_SRC_DIR}/imgui_tables.cpp
		${IMGUI_SRC_DIR}/imgui_widgets.cpp
		${IMGUI_SRC_DIR}/imgui.cpp
    )
    target_link_libraries(IMGUI Dwmapi)
endif ()
target_include_directories(IMGUI PUBLIC ${IMGUI_SRC_DIR})

# SPDLOG
set(SPDLOG_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/spdlog/include)
add_library(SPDLOG INTERFACE)
target_include_directories(SPDLOG INTERFACE ${SPDLOG_INC_DIR})

# IMGUIZMO
set(IMGUIZMO_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/imguizmo)
set(IMGUIZMO_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/imguizmo)
add_library(IMGUIZMO STATIC
		${IMGUIZMO_SRC_DIR}/ImCurveEdit.cpp
		${IMGUIZMO_SRC_DIR}/ImGradient.cpp
		${IMGUIZMO_SRC_DIR}/ImGuizmo.cpp
		${IMGUIZMO_SRC_DIR}/ImSequencer.cpp
)
target_link_libraries(IMGUIZMO IMGUI)
target_include_directories(IMGUIZMO PUBLIC ${IMGUIZMO_INC_DIR})

# IMGUI_CONSOLE
set(IMGUI_CONSOLE_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui_console/include)
set(IMGUI_CONSOLE_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui_console/src)
add_library(IMGUI_CONSOLE STATIC
		${IMGUI_CONSOLE_SRC_DIR}/imgui_console.cpp
)
target_link_libraries(IMGUI_CONSOLE IMGUI)
target_include_directories(IMGUI_CONSOLE PUBLIC ${IMGUI_CONSOLE_INC_DIR})

# TRACY
#option(TRACY_ENABLE "" ON)
#option(TRACY_ON_DEMAND "" ON)
#set(TRACY_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/tracy)
#set(TRACY_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/tracy)
#add_library(TRACY STATIC
#		${TRACY_SRC_DIR}/TracyClient.cpp
#)
#target_include_directories(TRACY PUBLIC ${TRACY_INC_DIR})
#add_subdirectory(${TRACY_INC_DIR})

FetchContent_Declare (
		tracy
		GIT_REPOSITORY https://github.com/wolfpld/tracy.git
		GIT_TAG v0.10
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable (tracy)