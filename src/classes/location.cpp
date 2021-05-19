#include "location.hpp"

Location::Location()
{
    exits.reserve(zarpa::max_exits);
    for (auto& exit : zarpa::exits) {
        exits[exit] = "";
    }
}