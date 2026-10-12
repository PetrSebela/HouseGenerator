#include "event_system.hpp"

#include <iostream>
#include <ostream>

#include "backends/imgui_impl_sdl3.h"

void EventSystem::RegisterEvent(Uint32 event, std::function<bool(SDL_Event*)> callback) {
    if (callbacks.count(event) == 0) {
        callbacks[event] = std::vector<std::function<bool(SDL_Event*)>>();
    }

    callbacks[event].push_back(callback);
}

void EventSystem::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // This probably should not be here, but i am too lazy to implement event wildcards
        ImGui_ImplSDL3_ProcessEvent(&event);
        auto io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantTextInput)
            continue;

        // for now, callbacks have priority based on their registration order
        for( const auto& callback : callbacks[event.type]) {
            if (callback(&event))
                break;
        }
    }
}
