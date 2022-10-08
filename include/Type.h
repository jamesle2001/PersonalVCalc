#pragma once

#include <string>

namespace vcalc {
    class Type {
    public:
        virtual std::string getName() = 0;
        virtual ~Type();
    };
}