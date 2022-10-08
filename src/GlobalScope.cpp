#include "GlobalScope.h"

namespace vcalc {
    GlobalScope::GlobalScope() : BaseScope(nullptr) {}

    std::string GlobalScope::getScopeName() {
        return "global";
    }
}

