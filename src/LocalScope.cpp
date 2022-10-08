#include "LocalScope.h"

namespace vcalc {
    LocalScope::LocalScope(std::shared_ptr<Scope> parent) : BaseScope(parent) {}

    std::string LocalScope::getScopeName() {
        return "local";
    }
}