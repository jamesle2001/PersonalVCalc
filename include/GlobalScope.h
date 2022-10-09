#pragma once

#include "BaseScope.h"


namespace vcalc {
    class GlobalScope : public BaseScope {
    public:
        GlobalScope();
        std::string getScopeName() override;
    };
}