#pragma once

#include "Symbol.h"
#include "Type.h"

namespace vcalc {
    class BuiltInTypeSymbol : public Symbol, public Type {
    public:
        BuiltInTypeSymbol(std::string name);
        std::string getName();
    };
}

