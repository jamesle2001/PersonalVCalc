#pragma once

#include <string>
#include <memory>

#include "Type.h"
#include "Scope.h"
#include "AST.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

namespace vcalc {
    class Scope; // forward declaration of Scope to resolve circular dependency

    class Symbol { // A generic programming language symbol
    public:
        std::string name;               // All symbols at least have a name
        std::shared_ptr<Type> type;
        std::shared_ptr<Scope> scope;   // All symbols know what scope contains them.
        llvm::AllocaInst *llvmAllocaInst;   // Reference to LLVM-ALLOCA

        Symbol(std::string name);
        Symbol(std::string name, std::shared_ptr<Type> type);
        virtual std::string getName();

        virtual std::string toString();
        virtual ~Symbol();
    };
}

