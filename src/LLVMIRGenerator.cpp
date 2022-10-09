#include "LLVMIRGenerator.h"
#include "VCalcParser.h"
#include "LocalScope.h"
#include "Symbol.h"
#include "VariableSymbol.h"

namespace vcalc {
    LLVMIRGenerator::LLVMIRGenerator(std::string &outputFileName) : globalCtx(), ir(globalCtx), mod(outputFileName, globalCtx), numExprAncestors(0) {
        llvm::FunctionType *mainFunctionType = llvm::FunctionType::get(ir.getVoidTy(), false);
        mainFunction = llvm::Function::Create(mainFunctionType, llvm::GlobalValue::ExternalLinkage, "main", mod);
    }

    void LLVMIRGenerator::visit(std::shared_ptr<AST> t) {
        if ( t->isNil() ) {
            visitChildren(t);
        } else {
            switch ( t->getNodeType() ) {
                case VCalcParser::BLOCK_TOKEN:
                    visitBLOCK_TOKEN(t);
                    break;
                case VCalcParser::VAR_DECLARATION_TOKEN:
                    visitVAR_DECLARATION_TOKEN(t);
                    break;
                case VCalcParser::ASSIGNMENT_TOKEN:
                    visitASSIGNMENT_TOKEN(t);
                    break;
                case VCalcParser::ID:
                    visitID(t);
                    break;
                case VCalcParser::EXPR_TOKEN:
                    // Keep track of number of expression ancestors for id references
                    numExprAncestors++;
                    visitEXPR_TOKEN(t);
                    numExprAncestors--;
                    break;
                case VCalcParser::ADD:
                case VCalcParser::SUB:
                case VCalcParser::MUL:
                case VCalcParser::DIV:
                case VCalcParser::GREATERTHAN:
                case VCalcParser::LESSTHAN:
                case VCalcParser::ISEQUAL:
                case VCalcParser::ISNOTEQUAL:
                    visitBinaryOperationToken(t);
                    break;
                case VCalcParser::RANGE:
                    visitRANGE(t);
                    break;
                case VCalcParser::INTEGER:
                    visitINTEGER(t);
                    break;
                case VCalcParser::PARENTHESIS_TOKEN:
                    visitPARENTHESIS_TOKEN(t);
                    break;
                case VCalcParser::PRINT:
                    visitPRINT(t);
                    break;
                case VCalcParser::GENERATOR_TOKEN:
                    visitGENERATOR_TOKEN(t);
                    break;
                case VCalcParser::FILTER_TOKEN:
                    visitFILTER_TOKEN(t);
                    break;
                case VCalcParser::LOOP_TOKEN:
                    visitLOOP_TOKEN(t);
                    break;
                case VCalcParser::CONDITIONAL_TOKEN:
                    visitCONDITIONAL_TOKEN(t);
                    break;
                default: // The other nodes we don't care about just have their children visited
                    visitChildren(t);
            }
        }
    }

    void LLVMIRGenerator::visitChildren(std::shared_ptr<AST> t) {
        for ( auto child : t->children ) visit(child);
    }

    void LLVMIRGenerator::visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        if (t->children[0]->token->getText() == "int") {
            t->symbol->llvmAllocaInst = ir.CreateAlloca(llvm::Type::getInt32Ty(globalCtx), nullptr);
            ir.CreateStore(t->children[2]->llvmValue, t->symbol->llvmAllocaInst);
        } else {
            t->symbol->llvmAllocaInst = llvm::dyn_cast<llvm::AllocaInst>(t->children[2]->llvmValue);
        }
    }
    void LLVMIRGenerator::visitASSIGNMENT_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        ir.CreateStore(t->children[1]->llvmValue, t->symbol->llvmAllocaInst);
    }

    void LLVMIRGenerator::visitBinaryOperationToken(std::shared_ptr<AST> t) {
        visitChildren(t);
        if (t->evalType->getName() == "int") {
            if (t->getNodeType() == VCalcParser::ADD) {
                t->llvmValue = ir.CreateAdd(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::SUB) {
                t->llvmValue = ir.CreateSub(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::MUL) {
                t->llvmValue = ir.CreateMul(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::DIV) {
                t->llvmValue = ir.CreateSDiv(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::GREATERTHAN) {
                t->llvmValue = ir.CreateICmpSGT(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::LESSTHAN) {
                t->llvmValue = ir.CreateICmpSLT(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::ISEQUAL) {
                t->llvmValue = ir.CreateICmpEQ(t->children[0]->llvmValue, t->children[1]->llvmValue);
            } else if (t->getNodeType() == VCalcParser::ISNOTEQUAL) {
                t->llvmValue = ir.CreateICmpNE(t->children[0]->llvmValue, t->children[1]->llvmValue);
            }
        } else {
            // TODO: Handle the case where the operations are with vector
        }
    }

    void LLVMIRGenerator::visitRANGE(std::shared_ptr<AST> t) {
        visitChildren(t);
        int lower_bound = std::stoi(t->children[0]->token->getText());
        int upper_bound = std::stoi(t->children[1]->token->getText());
        llvm::Type *intTy = llvm::Type::getInt32Ty(globalCtx);
        llvm::AllocaInst *allocaInst = ir.CreateAlloca(intTy, llvm::ConstantInt::get(intTy, upper_bound - lower_bound + 1, true));
        for (int i = lower_bound; i <= upper_bound; i++) {
            llvm::Value *pointerToElement = ir.CreateGEP(intTy, allocaInst, llvm::ArrayRef<llvm::Value *> {llvm::ConstantInt::get(intTy, lower_bound - i, true)});
            ir.CreateStore(llvm::ConstantInt::get(intTy, i, true), pointerToElement);
        }
        t->llvmValue = allocaInst;
    }

    void LLVMIRGenerator::visitID(std::shared_ptr<AST> t) {
        if ( numExprAncestors > 0 ) { // If an ID occurs within an expression, we have an ID reference
            if (t->evalType->getName() == "int") {
                t->llvmValue = ir.CreateLoad(llvm::Type::getInt32Ty(globalCtx), t->symbol->llvmAllocaInst);
            } else {
                t->llvmValue = t->symbol->llvmAllocaInst;
            }
        }
    }

    void LLVMIRGenerator::visitINTEGER(std::shared_ptr<AST> t) {
        t->llvmValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(globalCtx), std::stoi(t->token->getText()), true);
    }

    void LLVMIRGenerator::visitPARENTHESIS_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        t->llvmValue = t->children[0]->llvmValue;
    }

    void LLVMIRGenerator::visitGENERATOR_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        t->symbol->llvmAllocaInst = ir.CreateAlloca(llvm::Type::getInt32Ty(globalCtx), nullptr);
        visit(t->children[1]);
        llvm::AllocaInst *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
        // TODO
    }

    void LLVMIRGenerator::visitFILTER_TOKEN(std::shared_ptr<AST> t) {
        // TODO
        t->children[0]->symbol->llvmAllocaInst = ir.CreateAlloca(llvm::Type::getInt32Ty(globalCtx), nullptr);
        
    }

    void LLVMIRGenerator::visitPRINT(std::shared_ptr<AST> t) {
        // TODO
        visitChildren(t);
    }

    void LLVMIRGenerator::visitBLOCK_TOKEN(std::shared_ptr<AST> t) {
        auto *currentInsertBlock = ir.GetInsertBlock();
        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(globalCtx, "", mainFunction);
        ir.SetInsertPoint(basicBlock);
        visitChildren(t);
        ir.CreateRetVoid();
        ir.SetInsertPoint(currentInsertBlock);
    }

    void LLVMIRGenerator::visitEXPR_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        t->llvmValue = t->children[0]->llvmValue;
    }

    void LLVMIRGenerator::visitLOOP_TOKEN(std::shared_ptr<AST> t) {
        // TODO
        visitChildren(t);
    }

    void LLVMIRGenerator::visitCONDITIONAL_TOKEN(std::shared_ptr<AST> t) {
        // TODO
        visitChildren(t);
    }
}