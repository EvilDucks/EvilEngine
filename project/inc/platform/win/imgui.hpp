#pragma once
#include "imgui_impl_win32.h"

namespace IMGUI {

    bool __cdecl PlatformInitialize (void* value) {
        return ImGui_ImplWin32_Init (value);
    }

    void __cdecl PlatformShutdown () {
        ImGui_ImplWin32_Shutdown ();
    }

    void __cdecl PlatformNewFrame () {
        ImGui_ImplWin32_NewFrame ();
    }

}
