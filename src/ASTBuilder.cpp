#include "ASTBuilder.h"
#include "AST.h"

std::any ASTBuilder::visitCompilationUnit(VCalcParser::CompilationUnitContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>();
    for (auto stat: ctx->statement()) {
        t->addChild(visit(stat));
    }
    return t;
}

/* ^(VAR_DECLARATION_TOKEN type ID expression?) */
std::any ASTBuilder::visitVarDeclaration(VCalcParser::VarDeclarationContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::VAR_DECLARATION_TOKEN);
    t->addChild(visit(ctx->type()));
    t->addChild(std::make_shared<AST>(ctx->ID()->getSymbol()));
    if (ctx->expression()) {
        t->addChild(visit(ctx->expression()));
    }
    return t;
}

/* ^(ASSIGN postfixExpression expression) */
std::any ASTBuilder::visitAssignment(VCalcParser::AssignmentContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::ASSIGNMENT_TOKEN);
    t->addChild(std::make_shared<AST>(ctx->ID()->getSymbol()));
    t->addChild(visit(ctx->expression()));
    return t;
}

std::any ASTBuilder::visitConditional(VCalcParser::ConditionalContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::CONDITIONAL_TOKEN);
    t->addChild(visit(ctx->expression()));
    t->addChild(visit(ctx->block()));
    return t;
}

std::any ASTBuilder::visitLoop(VCalcParser::LoopContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::LOOP_TOKEN);
    t->addChild(visit(ctx->expression()));
    t->addChild(visit(ctx->block()));
    return t;
}

std::any ASTBuilder::visitPrint(VCalcParser::PrintContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::PRINT_TOKEN);
    t->addChild(visit(ctx->expression()));
    return t;
}

std::any ASTBuilder::visitGenerator(VCalcParser::GeneratorContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::GENERATOR_TOKEN);
    t->addChild(visit(ctx->ID()));
    t->addChild(visit(ctx->expression(0)));
    t->addChild(visit(ctx->expression(1)));
    return t;
}

std::any ASTBuilder::visitFilter(VCalcParser::FilterContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::FILTER_TOKEN);
    t->addChild(visit(ctx->ID()));
    t->addChild(visit(ctx->expression(0)));
    t->addChild(visit(ctx->expression(1)));
    return t;
}

/* ^(EXPR expr) */
std::any ASTBuilder::visitExpression(VCalcParser::ExpressionContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::EXPR_TOKEN);
    t->addChild(visit(ctx->expr()));
    return t;
}

std::any ASTBuilder::visitType(VCalcParser::TypeContext *ctx) {
    return std::make_shared<AST>(ctx->getStart()); // make AST node from the first token in this context
}

std::any ASTBuilder::visitBlock(VCalcParser::BlockContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::BLOCK_TOKEN);
    for (auto *stat : ctx->statement()) {
        t->addChild(visit(stat));
    }
    return t;
}

std::any ASTBuilder::visitParenthesis(VCalcParser::ParenthesisContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::PARENTHESIS_TOKEN);
    t->addChild(visit(ctx->expr()));
    return t;
}

std::any ASTBuilder::visitIndex(VCalcParser::IndexContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::INDEX_TOKEN);
    t->addChild(visit(ctx->expr(0)));
    t->addChild(visit(ctx->expr(1)));
    return t;
}

std::any ASTBuilder::visitRange(VCalcParser::RangeContext *ctx) {
    std::shared_ptr<AST> t = std::make_shared<AST>(VCalcParser::RANGE);
    t->addChild(visit(ctx->expr(0)));
    t->addChild(visit(ctx->expr(1)));
    return t;
}

std::any ASTBuilder::visitMulDiv(VCalcParser::MulDivContext *ctx) {
    std::shared_ptr<AST> t = nullptr;
    if (ctx->op->getType() == VCalcParser::MUL) {
        t = std::make_shared<AST>(VCalcParser::MUL);
    } else {
        t = std::make_shared<AST>(VCalcParser::DIV);
    }
    t->addChild(visit(ctx->expr(0)));
    t->addChild(visit(ctx->expr(1)));
    return t;
}

std::any ASTBuilder::visitAddSub(VCalcParser::AddSubContext *ctx) {
    std::shared_ptr<AST> t = nullptr;
    if (ctx->op->getType() == VCalcParser::ADD) {
        t = std::make_shared<AST>(VCalcParser::ADD);
    } else {
        t = std::make_shared<AST>(VCalcParser::SUB);
    }
    t->addChild(visit(ctx->expr(0)));
    t->addChild(visit(ctx->expr(1)));
    return t;
}

std::any ASTBuilder::visitGreaterThanLessThan(VCalcParser::GreaterThanLessThanContext *ctx) {
    std::shared_ptr<AST> t = nullptr;
    if (ctx->op->getType() == VCalcParser::LESSTHAN) {
        t = std::make_shared<AST>(VCalcParser::LESSTHAN);
    } else {
        t = std::make_shared<AST>(VCalcParser::GREATERTHAN);
    }
    t->addChild(visit(ctx->expr(0)));
    t->addChild(visit(ctx->expr(1)));
    return t;
}

std::any ASTBuilder::visitIsEqualIsNotEqual(VCalcParser::IsEqualIsNotEqualContext *ctx) {
    std::shared_ptr<AST> t = nullptr;
    if (ctx->op->getType() == VCalcParser::ISEQUAL) {
        t = std::make_shared<AST>(VCalcParser::ISEQUAL);
    } else {
        t = std::make_shared<AST>(VCalcParser::ISNOTEQUAL);
    }
    t->addChild(visit(ctx->expr(0)));
    t->addChild(visit(ctx->expr(1)));
    return t;
}

/* ID */
std::any ASTBuilder::visitIDAtom(VCalcParser::IDAtomContext *ctx) {
    return std::make_shared<AST>(ctx->ID()->getSymbol());
}

/* INT */
std::any ASTBuilder::visitIntegerAtom(VCalcParser::IntegerAtomContext *ctx) {
    return std::make_shared<AST>(ctx->INTEGER()->getSymbol());
}