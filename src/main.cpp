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
#include "ExpressionTypeComputation.h"
#include "LLVMIRGenerator.h"

#include <iostream>
#include <fstream>
#include <string>

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
  vcalc::ASTBuilder builder;
  std::shared_ptr<vcalc::AST> ast = std::any_cast<std::shared_ptr<vcalc::AST>>(builder.visit(tree));
  
  // Initialize the symbol table
  std::shared_ptr<vcalc::SymbolTable> symtab = std::make_shared<vcalc::SymbolTable>();

  // DefRef
  vcalc::DefRef defref(symtab);
	defref.visit(ast);

  // Expression Type Computation
  vcalc::ExpressionTypeComputation expressionTypeComputation(symtab);
  expressionTypeComputation.visit(ast);

  // LLVM IR Codegen Pass
  std::string outputFileName(argv[2]);
  vcalc::LLVMIRGenerator llvmIRGenerator(outputFileName);
  
  return 0;
}
