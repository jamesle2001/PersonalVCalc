#pragma once

#include "antlr4-runtime.h"
#include "Type.h"
#include "Scope.h"
#include "Symbol.h"

#include <vector>
#include <string>
#include <memory>

class Scope;   // forward declaration of Scope to resolve circular dependency
class Symbol;  // forward declaration of Scope to resolve circular dependency

class AST {
public:
    std::shared_ptr<antlr4::Token> token;       // From which token did we create node?
    std::vector<std::shared_ptr<AST>> children; // normalized list of children
    
    std::shared_ptr<Scope> scope;  // Populate by DefRef pass
    std::shared_ptr<Symbol> symbol; // Populate by DefRef pass
    std::shared_ptr<Type> evalType;  // Populate by Type pass
    std::shared_ptr<Type> promoteToType;  // Populate by Type pass

    AST(); // for making nil-rooted nodes
    AST(antlr4::Token* token);
    /** Create node from token type; used mainly for imaginary tokens */
    AST(size_t tokenType);

    /** External visitors execute the same action for all nodes
     *  with same node type while walking. */
    size_t getNodeType();
    
    void addChild(std::any t);
    void addChild(std::shared_ptr<AST> t);
    bool isNil();

    /** Compute string for single node */
    std::string toString();
    /** Compute string for a whole tree not just a node */
    std::string toStringTree();

    virtual ~AST();
};