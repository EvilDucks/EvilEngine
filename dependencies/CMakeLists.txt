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
