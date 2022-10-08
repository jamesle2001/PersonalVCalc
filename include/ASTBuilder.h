#include "VCalcBaseVisitor.h"

namespace vcalc {
    class ASTBuilder : public VCalcBaseVisitor {
    public:
        std::any visitCompilationUnit(VCalcParser::CompilationUnitContext *ctx) override;
        std::any visitVarDeclaration(VCalcParser::VarDeclarationContext *ctx) override;   
        std::any visitAssignment(VCalcParser::AssignmentContext *ctx) override;
        std::any visitConditional(VCalcParser::ConditionalContext *ctx) override;
        std::any visitLoop(VCalcParser::LoopContext *ctx) override;
        std::any visitPrint(VCalcParser::PrintContext *ctx) override;
        std::any visitExpression(VCalcParser::ExpressionContext *ctx) override;
        std::any visitType(VCalcParser::TypeContext *ctx) override;
        std::any visitBlock(VCalcParser::BlockContext *ctx) override;
        
        std::any visitParenthesis(VCalcParser::ParenthesisContext *ctx) override;
        std::any visitIndex(VCalcParser::IndexContext *ctx) override;
        std::any visitRange(VCalcParser::RangeContext *ctx) override;
        std::any visitMulDiv(VCalcParser::MulDivContext *ctx) override;
        std::any visitAddSub(VCalcParser::AddSubContext *ctx) override;
        std::any visitGreaterThanLessThan(VCalcParser::GreaterThanLessThanContext *ctx) override;
        std::any visitIsEqualIsNotEqual(VCalcParser::IsEqualIsNotEqualContext *ctx) override;
        std::any visitGenerator(VCalcParser::GeneratorContext *ctx) override;
        std::any visitFilter(VCalcParser::FilterContext *ctx) override;
        std::any visitIDAtom(VCalcParser::IDAtomContext *ctx) override;
        std::any visitIntegerAtom(VCalcParser::IntegerAtomContext *ctx) override;
    };
}
