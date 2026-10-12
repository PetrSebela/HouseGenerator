#ifndef _EVENT_SYSTEM_HPP_
#define _EVENT_SYSTEM_HPP_
#include <functional>
#include <map>
#include <set>
#include <vector>

#include "SDL3/SDL_events.h"

class EventSystem {
    std::map<Uint32, std::vector<std::function<bool(SDL_Event*)>>> callbacks;
    
    public:
        EventSystem() = default;
        ~EventSystem() = default;
        void RegisterEvent(Uint32 event, std::function<bool(SDL_Event*)> callback);
        void ProcessEvents();
};


#endif
