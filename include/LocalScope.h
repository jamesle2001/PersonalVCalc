#pragma once

#include <memory>

#include "BaseScope.h"

namespace vcalc {
    class LocalScope : public BaseScope {
    public:
        LocalScope(std::shared_ptr<Scope> parent);
        std::string getScopeName() override;
    };
}
