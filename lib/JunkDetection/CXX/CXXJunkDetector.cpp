#include "mull/JunkDetection/CXX/CXXJunkDetector.h"

#include "mull/Config/RawConfig.h"
#include "mull/MutationPoint.h"
#include "mull/Mutators/Mutator.h"

#include "mull/JunkDetection/CXX/Visitors/AndOrReplacementVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/BinaryVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/MathDivVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/MathMulVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/NegateConditionVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/RemoveVoidFunctionVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/ReplaceAssignmentVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/ReplaceCallVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/ScalarValueVisitor.h"
#include "mull/JunkDetection/CXX/Visitors/UnaryVisitor.h"

using namespace mull;

template <typename Visitor>
static bool isJunkMutation(ASTStorage &storage, MutationPoint *point) {
  auto ast = storage.findAST(point);
  auto location = ast->getLocation(point);

  if (ast->isInSystemHeader(location)) {
    return true;
  }

  VisitorParameters parameters = {.sourceManager = ast->getSourceManager(),
                                  .sourceLocation = location,
                                  .astContext = ast->getASTContext()};
  Visitor visitor(parameters);
  visitor.TraverseDecl(ast->getASTContext().getTranslationUnitDecl());

  if (clang::Expr *mutantExpression = visitor.foundMutant()) {
    storage.setMutantASTNode(point, mutantExpression);
    return false;
  }

  return true;
}

CXXJunkDetector::CXXJunkDetector(ASTStorage &astStorage)
    : astStorage(astStorage) {}

bool CXXJunkDetector::isJunk(MutationPoint *point) {
  if (point->getSourceLocation().isNull()) {
    return true;
  }

  switch (point->getMutator()->mutatorKind()) {
  case MutatorKind::MathMulMutator:
    return isJunkMutation<MathMulVisitor>(astStorage, point);
  case MutatorKind::MathDivMutator:
    return isJunkMutation<MathDivVisitor>(astStorage, point);
  case MutatorKind::RemoveVoidFunctionMutator:
    return isJunkMutation<RemoveVoidFunctionVisitor>(astStorage, point);
  case MutatorKind::ReplaceCallMutator:
    return isJunkMutation<ReplaceCallVisitor>(astStorage, point);
  case MutatorKind::NegateMutator:
    return isJunkMutation<NegateConditionVisitor>(astStorage, point);
  case MutatorKind::AndOrReplacementMutator:
    return isJunkMutation<AndOrReplacementVisitor>(astStorage, point);
  case MutatorKind::ScalarValueMutator:
    return isJunkMutation<ScalarValueVisitor>(astStorage, point);
  case MutatorKind::ReplaceAssignmentMutator:
    return isJunkMutation<ReplaceAssignmentVisitor>(astStorage, point);

  case MutatorKind::CXX_Relation_LessThanToLessOrEqual:
    return isJunkMutation<cxx::LessThanVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_LessOrEqualToLessThan:
    return isJunkMutation<cxx::LessOrEqualVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_GreaterThanToGreaterOrEqual:
    return isJunkMutation<cxx::GreaterThanVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_GreaterOrEqualToGreaterThan:
    return isJunkMutation<cxx::GreaterOrEqualVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_EqualToNotEqual:
    return isJunkMutation<cxx::EqualVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_NotEqualToEqual:
    return isJunkMutation<cxx::NotEqualVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_GreaterThanToLessOrEqual:
    return isJunkMutation<cxx::GreaterThanVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_GreaterOrEqualToLessThan:
    return isJunkMutation<cxx::GreaterOrEqualVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_LessThanToGreaterOrEqual:
    return isJunkMutation<cxx::LessThanVisitor>(astStorage, point);
  case MutatorKind::CXX_Relation_LessOrEqualToGreaterThan:
    return isJunkMutation<cxx::LessOrEqualVisitor>(astStorage, point);

  case MutatorKind::CXX_Arithmetic_AddToSub:
    return isJunkMutation<cxx::AddVisitor>(astStorage, point);
  case MutatorKind::CXX_Arithmetic_AddAssignToSubAssign:
    return isJunkMutation<cxx::AddAssignVisitor>(astStorage, point);
  case MutatorKind::CXX_Arithmetic_PreIncToPreDec:
    return isJunkMutation<cxx::PreIncVisitor>(astStorage, point);
  case MutatorKind::CXX_Arithmetic_PostIncToPostDec:
    return isJunkMutation<cxx::PostIncVisitor>(astStorage, point);

  case MutatorKind::CXX_Arithmetic_SubToAdd:
    return isJunkMutation<cxx::SubVisitor>(astStorage, point);
  case MutatorKind::CXX_Arithmetic_SubAssignToAddAssign:
    return isJunkMutation<cxx::SubAssignVisitor>(astStorage, point);
  case MutatorKind::CXX_Arithmetic_PreDecToPreInc:
    return isJunkMutation<cxx::PreDecVisitor>(astStorage, point);
  case MutatorKind::CXX_Arithmetic_PostDecToPostInc:
    return isJunkMutation<cxx::PostDecVisitor>(astStorage, point);
  default:
    return false;
  }
}
