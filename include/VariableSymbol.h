#pragma once

#include <memory>

#include "Symbol.h"
#include "Type.h"

namespace vcalc {
    class VariableSymbol : public Symbol {
    public:
        VariableSymbol(std::string name, std::shared_ptr<Type> type);
    };
}