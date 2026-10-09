#include "event_system.hpp"

#include <iostream>
#include <ostream>

#include "backends/imgui_impl_sdl3.h"

void EventSystem::RegisterEvent(Uint32 event, std::function<void(SDL_Event*)> callback) {
    if (callbacks.count(event) == 0) {
        callbacks[event] = std::vector<std::function<void(SDL_Event*)>>();
    }

    callbacks[event].push_back(callback);
}

void EventSystem::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // This probably should not be here, but i am too lazy to implement event wildcards
        ImGui_ImplSDL3_ProcessEvent(&event);

        // std::cout << "event occured" << std::endl;
        for( const auto& callback : callbacks[event.type]) {
            callback(&event);
        }
    }
}
