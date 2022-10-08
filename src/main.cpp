#include "VCalcLexer.h"
#include "VCalcParser.h"

#include "ANTLRFileStream.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"
#include "tree/ParseTreeWalker.h"

#include "AST.h"
#include "ASTBuilder.h"
#include "DefRef.h"
#include "SymbolTable.h"

#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "Missing required argument.\n"
              << "Required arguments: <input file path> <output file path>\n";
    return 1;
  }

  // Open the file then parse and lex it.
  antlr4::ANTLRFileStream afs;
  afs.loadFromFile(argv[1]);
  vcalc::VCalcLexer lexer(&afs);
  antlr4::CommonTokenStream tokens(&lexer);
  vcalc::VCalcParser parser(&tokens);

  // Get the root of the parse tree. Use your base rule name.
  antlr4::tree::ParseTree *tree = parser.compilationUnit();

  // Build AST
  ASTBuilder builder;
  std::shared_ptr<AST> ast = std::any_cast<std::shared_ptr<AST>>(builder.visit(tree));
  
  // Initialize the symbol table
  std::shared_ptr<SymbolTable> symtab = std::make_shared<SymbolTable>();

  // DefRef
  DefRef defref(symtab);
	defref.visit(ast);

  return 0;
}
