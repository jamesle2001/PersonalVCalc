#include "SymbolTable.h"

#include "BuiltInTypeSymbol.h"

namespace vcalc {
    void SymbolTable::initTypeSystem() {
        globals->define(std::make_shared<BuiltInTypeSymbol>("int"));
        globals->define(std::make_shared<BuiltInTypeSymbol>("vector"));
    }

    SymbolTable::SymbolTable() : globals(std::make_shared<GlobalScope>()) { 
        initTypeSystem(); 
    }

    std::string SymbolTable::toString() {
        return globals->toString();
    }
}

