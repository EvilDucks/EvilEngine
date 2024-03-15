#pragma once
#include "imgui_impl_glfw.h"

namespace IMGUI {

    bool __cdecl PlatformInitialize (GLFWwindow* value) {
        return ImGui_ImplGlfw_InitForOpenGL (value, true);
    }

    void __cdecl PlatformShutdown () {
        ImGui_ImplGlfw_Shutdown ();
    }

    void __cdecl PlatformNewFrame () {
        ImGui_ImplGlfw_NewFrame ();
    }

}
