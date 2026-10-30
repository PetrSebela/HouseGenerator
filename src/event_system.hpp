#pragma once

#include <functional>
#include <map>
#include <vector>

#include "SDL3/SDL_events.h"

/**
 * Basic event system
 * This system supports basic input masking (return true from callback if said callback should consume said event)
 * This system does technically support priority (callback registration order)
 */
class EventSystem {
    // Map of event and list of callback for its processing
    std::map<Uint32, std::vector<std::function<bool(SDL_Event*)>>> callbacks;
    
public:
    EventSystem() = default;
    ~EventSystem() = default;

    /**
     * Registers event callback with event system
     * @param event SDL event type
     * @param callback callback function (return true if callback should consume said event)
     */
    void RegisterEvent(Uint32 event, std::function<bool(SDL_Event*)> callback);

    /**
     * Polls event and dispatches coresponding event callback
     */
    void ProcessEvents();
};