#include "LLVMIRGenerator.h"
#include "VCalcParser.h"
#include "LocalScope.h"
#include "Symbol.h"
#include "VariableSymbol.h"

namespace vcalc {
    LLVMIRGenerator::LLVMIRGenerator(std::string &outputFileName) : globalCtx(), ir(globalCtx), mod("vcalc", globalCtx), numBasicBlocks(0), numVariables(0), numExprAncestors(0), outputFileName(outputFileName) {
        llvm::FunctionType *mainFunctionType = llvm::FunctionType::get(ir.getVoidTy(), false);
        mainFunction = llvm::Function::Create(mainFunctionType, llvm::GlobalValue::ExternalLinkage, "main", mod);
        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(globalCtx, "BasicBlock" + std::to_string(++numBasicBlocks), mainFunction);
        ir.SetInsertPoint(basicBlock);
    }

    void LLVMIRGenerator::visit(std::shared_ptr<AST> t) {
        if ( t->isNil() ) {
            visitChildren(t);
        } else {
            switch ( t->getNodeType() ) {
                case VCalcParser::BLOCK_TOKEN:
                    visitBLOCK_TOKEN(t);
                    break;
                case VCalcParser::PRINT:
                    visitPRINT(t);
                    break;
                case VCalcParser::VAR_DECLARATION_TOKEN:
                    visitVAR_DECLARATION_TOKEN(t);
                    break;
                case VCalcParser::ASSIGNMENT_TOKEN:
                    visitASSIGNMENT_TOKEN(t);
                    break;
                case VCalcParser::LOOP_TOKEN:
                    visitLOOP_TOKEN(t);
                    break;
                case VCalcParser::CONDITIONAL_TOKEN:
                    visitCONDITIONAL_TOKEN(t);
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
                case VCalcParser::ID:
                    visitID(t);
                    break;
                case VCalcParser::GENERATOR_TOKEN:
                    visitGENERATOR_TOKEN(t);
                    break;
                case VCalcParser::FILTER_TOKEN:
                    visitFILTER_TOKEN(t);
                    break;
                case VCalcParser::INDEX_TOKEN:
                    visitINDEX_TOKEN(t);
                    break;
                default: // The other nodes we don't care about just have their children visited
                    visitChildren(t);
            }
        }
    }

    void LLVMIRGenerator::visitChildren(std::shared_ptr<AST> t) {
        for ( auto child : t->children ) visit(child);
    }

    void LLVMIRGenerator::visitPRINT(std::shared_ptr<AST> t) {
        // TODO
        visitChildren(t);
        if (t->children[0]->evalType->getName() == "int") {

        } else {

        }
    }

    void LLVMIRGenerator::visitBLOCK_TOKEN(std::shared_ptr<AST> t) {
        auto *currentInsertBlock = ir.GetInsertBlock();
        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(globalCtx, "BasicBlock" + std::to_string(++numBasicBlocks), mainFunction);
        ir.SetInsertPoint(basicBlock);
        visitChildren(t);
        ir.CreateRetVoid();
        ir.SetInsertPoint(currentInsertBlock);
    }

    void LLVMIRGenerator::visitVAR_DECLARATION_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        if (t->children[0]->token->getText() == "int") {
            t->symbol->llvmAllocaInst = ir.CreateAlloca(llvm::Type::getInt32Ty(globalCtx), nullptr, "Variable" + std::to_string(++numVariables));
            ir.CreateStore(t->children[2]->llvmValue, t->symbol->llvmAllocaInst);
        } else {
            t->symbol->llvmAllocaInst = llvm::dyn_cast<llvm::AllocaInst>(t->children[2]->llvmValue);
        }
    }
    void LLVMIRGenerator::visitASSIGNMENT_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        ir.CreateStore(t->children[1]->llvmValue, t->symbol->llvmAllocaInst);
    }

    void LLVMIRGenerator::visitLOOP_TOKEN(std::shared_ptr<AST> t) {
        // TODO
        visitChildren(t);
    }

    void LLVMIRGenerator::visitCONDITIONAL_TOKEN(std::shared_ptr<AST> t) {
        // TODO
        visitChildren(t);
    }


    void LLVMIRGenerator::visitEXPR_TOKEN(std::shared_ptr<AST> t) {
        visitChildren(t);
        t->llvmValue = t->children[0]->llvmValue;
    }

    void LLVMIRGenerator::visitBinaryOperationToken(std::shared_ptr<AST> t) {
        visitChildren(t);
        llvm::Type *intTy = llvm::Type::getInt32Ty(globalCtx);
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
                t->llvmValue = ir.CreateIntCast(ir.CreateICmpSGT(t->children[0]->llvmValue, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
            } else if (t->getNodeType() == VCalcParser::LESSTHAN) {
                t->llvmValue = ir.CreateIntCast(ir.CreateICmpSLT(t->children[0]->llvmValue, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
            } else if (t->getNodeType() == VCalcParser::ISEQUAL) {
                t->llvmValue = ir.CreateIntCast(ir.CreateICmpEQ(t->children[0]->llvmValue, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
            } else if (t->getNodeType() == VCalcParser::ISNOTEQUAL) {
                t->llvmValue = ir.CreateIntCast(ir.CreateICmpNE(t->children[0]->llvmValue, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
            }
        } else {
            // Handle the case where the operations are with Arrays
            if (t->children[0]->evalType->getName() == "vector" && t->children[1]->evalType->getName() == "vector") {
                llvm::AllocaInst *op1Array = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
                llvm::AllocaInst *op2Array = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
                llvm::Value *op1ArraySize = op1Array->getArraySize();

                // Cast from *Value to integer
                int array1Size;
                if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(op1ArraySize)) {
                    if (CI->getBitWidth() <= 32) {
                        array1Size = CI->getSExtValue();
                    }
                }
                // Store the allocaInst of this FILTER AST node
                t->symbol->llvmAllocaInst = ir.CreateAlloca(
                    llvm::Type::getInt32Ty(globalCtx), 
                    op1ArraySize, 
                    "Variable" + std::to_string(++numVariables)
                );

                for (int i = 0; i < array1Size; i++) {
                    llvm::Value *array1ElementRef = ir.CreateGEP(
                        intTy, 
                        op1Array, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    auto *array1Element = ir.CreateLoad(intTy, array1ElementRef);

                    llvm::Value *array2ElementRef = ir.CreateGEP(
                        intTy, 
                        op2Array, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    auto *array2Element = ir.CreateLoad(intTy, array2ElementRef);

                    llvm::Value *result = llvm::ConstantInt::get(intTy, 0, true);  // Dummy Value
                    if (t->getNodeType() == VCalcParser::ADD) {
                        result = ir.CreateAdd(array1Element, array2Element);
                    } else if (t->getNodeType() == VCalcParser::SUB) {
                        result = ir.CreateSub(array1Element, array2Element);
                    } else if (t->getNodeType() == VCalcParser::MUL) {
                        result = ir.CreateMul(array1Element, array2Element);
                    } else if (t->getNodeType() == VCalcParser::DIV) {
                        result = ir.CreateSDiv(array1Element, array2Element);
                    } else if (t->getNodeType() == VCalcParser::GREATERTHAN) {
                        result = ir.CreateIntCast(ir.CreateICmpSGT(array1Element, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::LESSTHAN) {
                        result = ir.CreateIntCast(ir.CreateICmpSLT(array1Element, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::ISEQUAL) {
                        result = ir.CreateIntCast(ir.CreateICmpEQ(array1Element, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::ISNOTEQUAL) {
                        result = ir.CreateIntCast(ir.CreateICmpNE(array1Element, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    }

                    llvm::Value *resultElementRef = ir.CreateGEP(
                        intTy, 
                        t->symbol->llvmAllocaInst, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    ir.CreateStore(result, resultElementRef);
                }
            } else if (t->children[0]->evalType->getName() == "vector" && t->children[1]->evalType->getName() == "int") {
                llvm::AllocaInst *op1Array = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
                llvm::Value *op1ArraySize = op1Array->getArraySize();

                // Cast from *Value to integer
                int array1Size;
                if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(op1ArraySize)) {
                    if (CI->getBitWidth() <= 32) {
                        array1Size = CI->getSExtValue();
                    }
                }
                // Store the allocaInst of this FILTER AST node
                t->symbol->llvmAllocaInst = ir.CreateAlloca(
                    llvm::Type::getInt32Ty(globalCtx), 
                    op1ArraySize, 
                    "Variable" + std::to_string(++numVariables)
                );

                for (int i = 0; i < array1Size; i++) {
                    llvm::Value *array1ElementRef = ir.CreateGEP(
                        intTy, 
                        op1Array, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    auto *array1Element = ir.CreateLoad(intTy, array1ElementRef);

                    llvm::Value *result = llvm::ConstantInt::get(intTy, 0, true);  // Dummy Value
                    if (t->getNodeType() == VCalcParser::ADD) {
                        result = ir.CreateAdd(array1Element, t->children[1]->llvmValue);
                    } else if (t->getNodeType() == VCalcParser::SUB) {
                        result = ir.CreateSub(array1Element, t->children[1]->llvmValue);
                    } else if (t->getNodeType() == VCalcParser::MUL) {
                        result = ir.CreateMul(array1Element, t->children[1]->llvmValue);
                    } else if (t->getNodeType() == VCalcParser::DIV) {
                        result = ir.CreateSDiv(array1Element, t->children[1]->llvmValue);
                    } else if (t->getNodeType() == VCalcParser::GREATERTHAN) {
                        result = ir.CreateIntCast(ir.CreateICmpSGT(array1Element, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::LESSTHAN) {
                        result = ir.CreateIntCast(ir.CreateICmpSLT(array1Element, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::ISEQUAL) {
                        result = ir.CreateIntCast(ir.CreateICmpEQ(array1Element, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::ISNOTEQUAL) {
                        result = ir.CreateIntCast(ir.CreateICmpNE(array1Element, t->children[1]->llvmValue), llvm::Type::getInt32Ty(globalCtx), true);
                    }

                    llvm::Value *resultElementRef = ir.CreateGEP(
                        intTy, 
                        t->symbol->llvmAllocaInst, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    ir.CreateStore(result, resultElementRef);
                }
            } else if (t->children[0]->evalType->getName() == "int" && t->children[1]->evalType->getName() == "vector") {
                llvm::AllocaInst *op2Array = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
                llvm::Value *op2ArraySize = op2Array->getArraySize();

                // Cast from *Value to integer
                int array2Size;
                if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(op2ArraySize)) {
                    if (CI->getBitWidth() <= 32) {
                        array2Size = CI->getSExtValue();
                    }
                }
                // Store the allocaInst of this FILTER AST node
                t->symbol->llvmAllocaInst = ir.CreateAlloca(
                    llvm::Type::getInt32Ty(globalCtx), 
                    op2ArraySize, 
                    "Variable" + std::to_string(++numVariables)
                );

                for (int i = 0; i < array2Size; i++) {
                    llvm::Value *array2ElementRef = ir.CreateGEP(
                        intTy, 
                        op2Array, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    auto *array2Element = ir.CreateLoad(intTy, array2ElementRef);

                    llvm::Value *result = llvm::ConstantInt::get(intTy, 0, true);  // Dummy Value
                    if (t->getNodeType() == VCalcParser::ADD) {
                        result = ir.CreateAdd(t->children[0]->llvmValue, array2Element);
                    } else if (t->getNodeType() == VCalcParser::SUB) {
                        result = ir.CreateSub(t->children[0]->llvmValue, array2Element);
                    } else if (t->getNodeType() == VCalcParser::MUL) {
                        result = ir.CreateMul(t->children[0]->llvmValue, array2Element);
                    } else if (t->getNodeType() == VCalcParser::DIV) {
                        result = ir.CreateSDiv(t->children[0]->llvmValue, array2Element);
                    } else if (t->getNodeType() == VCalcParser::GREATERTHAN) {
                        result = ir.CreateIntCast(ir.CreateICmpSGT(t->children[0]->llvmValue, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::LESSTHAN) {
                        result = ir.CreateIntCast(ir.CreateICmpSLT(t->children[0]->llvmValue, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::ISEQUAL) {
                        result = ir.CreateIntCast(ir.CreateICmpEQ(t->children[0]->llvmValue, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    } else if (t->getNodeType() == VCalcParser::ISNOTEQUAL) {
                        result = ir.CreateIntCast(ir.CreateICmpNE(t->children[0]->llvmValue, array2Element), llvm::Type::getInt32Ty(globalCtx), true);
                    }

                    llvm::Value *resultElementRef = ir.CreateGEP(
                        intTy, 
                        t->symbol->llvmAllocaInst, 
                        llvm::ArrayRef<llvm::Value *> { 
                            llvm::ConstantInt::get(intTy, 0, true), 
                            llvm::ConstantInt::get(intTy, i, true) 
                        }
                    );
                    ir.CreateStore(result, resultElementRef);
                }
            }
        }
    }

    void LLVMIRGenerator::visitRANGE(std::shared_ptr<AST> t) {
        visitChildren(t);
        int lower_bound = std::stoi(t->children[0]->token->getText());
        int upper_bound = std::stoi(t->children[1]->token->getText());
        llvm::Type *intTy = llvm::Type::getInt32Ty(globalCtx);
        llvm::AllocaInst *allocaInst = ir.CreateAlloca(intTy, llvm::ConstantInt::get(intTy, upper_bound - lower_bound + 1, true), "Variable" + std::to_string(++numVariables));
        for (int i = lower_bound; i <= upper_bound; i++) {
            llvm::Value *pointerToElement = ir.CreateGEP(
                intTy, 
                allocaInst, 
                llvm::ArrayRef<llvm::Value *> { 
                        llvm::ConstantInt::get(intTy, 0, true), 
                        llvm::ConstantInt::get(intTy, lower_bound - i, true) 
                }
            );
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
        visit(t->children[1]);  // Visit the Domain
        llvm::Type *intTy = llvm::Type::getInt32Ty(globalCtx);
        llvm::AllocaInst *domainRef = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
        llvm::Value *domainSize = domainRef->getArraySize();

        // Store the allocaInst of this FILTER AST node
        t->symbol->llvmAllocaInst = ir.CreateAlloca(llvm::Type::getInt32Ty(globalCtx), domainSize, "Variable" + std::to_string(++numVariables));
        
        // Cast from *Value to integer
        int domainSizeInteger;
        if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(domainSize)) {
            if (CI->getBitWidth() <= 32) {
                domainSizeInteger = CI->getSExtValue();
            }
        }

        std::vector<llvm::Value *> llvmValues;

        for (int i = 0; i <= domainSizeInteger; i++) {
            // Get the element of the domain
            llvm::Value *pointerToDomainElement = ir.CreateGEP(
                intTy, 
                domainRef, 
                llvm::ArrayRef<llvm::Value *> { 
                    llvm::ConstantInt::get(intTy, 0, true), 
                    llvm::ConstantInt::get(intTy, i, true) 
                }
            );
            t->children[0]->llvmValue = ir.CreateLoad(intTy, pointerToDomainElement);
            visit(t->children[2]);  // Computation based on the value from an element of domain

            // Storing result
            llvm::Value *pointerToResultArrayElement = ir.CreateGEP(
                intTy, 
                t->symbol->llvmAllocaInst, 
                llvm::ArrayRef<llvm::Value *> { 
                    llvm::ConstantInt::get(intTy, 0, true), 
                    llvm::ConstantInt::get(intTy, i, true) 
                }
            );
            ir.CreateStore(t->children[2]->llvmValue, pointerToResultArrayElement);
        }
    }

    void LLVMIRGenerator::visitFILTER_TOKEN(std::shared_ptr<AST> t) {
        visit(t->children[1]);  // Visit the Domain
        llvm::Type *intTy = llvm::Type::getInt32Ty(globalCtx);
        llvm::AllocaInst *domainRef = llvm::dyn_cast<llvm::AllocaInst>(t->children[1]->llvmValue);
        llvm::Value *domainSize = domainRef->getArraySize();
        
        // Cast from *Value to integer
        int domainSizeInteger;
        if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(domainSize)) {
            if (CI->getBitWidth() <= 32) {
                domainSizeInteger = CI->getSExtValue();
            }
        }

        std::vector<llvm::Value *> llvmValueVector;
        for (int i = 0; i <= domainSizeInteger; i++) {
            // Get the element of the domain
            llvm::Value *pointerToDomainElement = ir.CreateGEP(
                intTy, 
                domainRef, 
                llvm::ArrayRef<llvm::Value *> { 
                    llvm::ConstantInt::get(intTy, 0, true), 
                    llvm::ConstantInt::get(intTy, i, true) 
                }
            );
            t->children[0]->llvmValue = ir.CreateLoad(intTy, pointerToDomainElement);
            visit(t->children[2]);  // Computation based on the value from an element of domain

            llvm::Value *llvmPredicateResult = ir.CreateICmpEQ(t->children[2]->llvmValue, llvm::ConstantInt::get(intTy, 1, true));
            // Cast from *Value to integer/boolean
            int predicateResult;
            if (llvm::ConstantInt* CIPredicate = llvm::dyn_cast<llvm::ConstantInt>(llvmPredicateResult)) {
                if (CIPredicate->getBitWidth() <= 32) {
                    predicateResult = CIPredicate->getSExtValue();
                }
            }

            if (predicateResult == 1) {
                // If the domain variable satisfies the predicate, i.e., t->children[0]->llvmValue is true
                llvmValueVector.push_back(t->children[0]->llvmValue);
            }
        }

        // Store the allocaInst of this FILTER AST node
        t->symbol->llvmAllocaInst = ir.CreateAlloca(
            llvm::Type::getInt32Ty(globalCtx), 
            llvm::ConstantInt::get(intTy, llvmValueVector.size(), true), 
            "Variable" + std::to_string(++numVariables)
        );

        for (unsigned int i = 0; i < llvmValueVector.size(); i++) {
            // Storing result
            llvm::Value *pointerToResultArrayElement = ir.CreateGEP(
                intTy, 
                t->symbol->llvmAllocaInst, 
                llvm::ArrayRef<llvm::Value *> { 
                    llvm::ConstantInt::get(intTy, 0, true), 
                    llvm::ConstantInt::get(intTy, i, true) 
                }
            );
            ir.CreateStore(llvmValueVector[i], pointerToResultArrayElement);
        }
    }

    void LLVMIRGenerator::visitINDEX_TOKEN(std::shared_ptr<AST> t) {
        llvm::AllocaInst *arrayRef = llvm::dyn_cast<llvm::AllocaInst>(t->children[0]->llvmValue);
        llvm::Value *index = t->children[1]->llvmValue;
        llvm::Type *intTy = llvm::Type::getInt32Ty(globalCtx);
        llvm::Value *pointerToElement = ir.CreateGEP(
            intTy, 
            arrayRef, 
            llvm::ArrayRef<llvm::Value *> { 
                llvm::ConstantInt::get(intTy, 0, true), 
                index 
            }
        );
        t->llvmValue = ir.CreateLoad(intTy, pointerToElement);
    }
}