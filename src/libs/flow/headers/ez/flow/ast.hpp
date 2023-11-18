#pragma once

#include <ez/enum.hpp>
#include <ez/option.hpp>
#include <ez/tuple.hpp>

#include <boost/variant/recursive_wrapper.hpp>

#include <string>
#include <vector>

namespace ez::flow::ast {

struct Located {
    uint start_position = 0;
    uint end_position = 0;
};

template <typename T>
using Forward = boost::recursive_wrapper<T>;

template <typename T, typename Tag>
struct Element : public T, Located {
    using T::T;
    using T::operator=;
};

template <typename T, typename Tag>
using Vector = Element<std::vector<T>, Tag>;

template <typename T, typename Tag>
struct PodElement : Located {
    T value;
    operator T() const { return value; }
};

//////

using Identifier = Element<std::string, struct IdentifierTag>;
using IdentifierPath = Vector<Identifier, struct IdentifierPathTag>;

//////

using SingleQuoteString = Element<std::string, struct SingleQuoteStringTag>;
using TripleQuoteString = Element<std::string, struct TripleQuoteStringTag>;

using String = Enum<SingleQuoteString, TripleQuoteString>;

//////

using Integer = PodElement<std::int64_t, struct IntegerTag>;
using Real = PodElement<double, struct RealTag>;

enum class DurationUnit { Ms, Sec, Min, Day, Week, Month };

struct Duration : Located {
    std::uint64_t count;
    DurationUnit unit;
};
// clang-format off
using Literal = Enum <
    SingleQuoteString,
    TripleQuoteString,
    bool,
    Integer,
    Real,
    Duration
>;
// clang-format on

//////

struct Declaration : Located {
    Identifier name;
    Identifier type;
};

using DeclarationList = Vector<Declaration, struct DeclarationListTag>;

struct Signature : Located {
    DeclarationList inputs;
    Option<Identifier> return_type;
};

struct FunctionCall;
struct Array;
struct Dictionary;
struct Expression;
struct AwaitExpression;
struct TryExpression;

struct UnaryExpression;

using FunctionCallFwd = Forward<FunctionCall>;
using ArrayFwd = Forward<Array>;
using DictionaryFwd = Forward<Dictionary>;
using ExpressionFwd = Forward<Expression>;
using AwaitExpressionFwd = Forward<AwaitExpression>;
using TryExpressionFwd = Forward<TryExpression>;

// clang-format off
using PrimaryExpression = Enum <
    IdentifierPath,
    Literal,
    FunctionCallFwd,
    ArrayFwd,
    DictionaryFwd,
    ExpressionFwd,
    AwaitExpressionFwd,
    TryExpressionFwd

>;
// clang-format on

struct Array : Vector<Expression, struct ArrayTag> {
    using Super = Vector<Expression, struct ArrayTag>;
    using Super::Super;
    using Super::operator=;
};

/////////////////////////////////////////////////////

template <typename T, typename Operator>
struct Operation : Located {
    Operator op;
    T term;
};

template <typename T, typename Operator>
using Operations = Vector<Operation<T, Operator>, struct OperationsTag>;

/////////////////////////////////////////////////////

enum class UnaryOperator { Plus, Minus, Not };
enum class MultiplicativeOperator { Mult, Div };
enum class AdditiveOperator { Plus, Minus };
enum class RelationalOperator { Less, LessEq, Greater, GreaterEq };
enum class EqualityOperator { Eq, NotEq, In };
enum class LogicalOperator { And, Or };

/////////////////////////////////////////////////////

struct UnaryExpression : Located {
    Option<UnaryOperator> op;
    PrimaryExpression expression;
};

template <typename Term, typename Operator>
struct BinaryExpression : Located {
    Term first;
    Operations<Term, Operator> terms;
};

using MultiplicativeExpression = BinaryExpression<UnaryExpression, MultiplicativeOperator>;
using AdditiveExpression = BinaryExpression<MultiplicativeExpression, AdditiveOperator>;
using RelationalExpression = BinaryExpression<AdditiveExpression, RelationalOperator>;
using EqualityExpression = BinaryExpression<RelationalExpression, EqualityOperator>;
using LogicalExpression = BinaryExpression<EqualityExpression, LogicalOperator>;

struct Expression : LogicalExpression {};

//////////

struct FunctionArgument : Located {
    Option<Identifier> name;
    Expression value;
};

struct FunctionCall : Located {
    IdentifierPath path;
    Vector<FunctionArgument, struct FunctionCallArsTag> arguments;
};

struct Assignment : Located {
    Identifier name;
    Expression value;
};

struct Dictionary : Vector<Assignment, struct DictionaryTag> {
    using Super = Vector<Assignment, struct DictionaryTag>;
    using Super::Super;
    using Super::operator=;
};

struct AwaitExpression : Located {
    Expression expression;
    Expression duration;
};

struct TryExpression : Located {
    Expression expression;
};

///////////////////////////////////////////////////////////////////////////////

struct AnonymousObjectConstruction : Located {
    Identifier type;
    Dictionary arguments;
};

struct VariableDeclaration : Located {
    Identifier name;
    Identifier type;
    Option<Expression> expression;
};

struct AssignmentStatement : Located {
    IdentifierPath path;
    Expression expression;
};

/// built in

struct ImportStatement : Located {
    IdentifierPath path;
};

struct DelayStatement : Located {
    Expression expression;
};

struct ReturnStatement : Located {
    Expression expression;
};

struct RaiseStatement : Located {
    Expression expression;
};

struct IfBlock;
using IfBlockFwd = Forward<IfBlock>;

struct WorkflowDefinition;
using WorkflowDefinitionFwd = Forward<WorkflowDefinition>;

///

// clang-format off
using WorkflowStatement = Enum <
    ImportStatement,
    VariableDeclaration,
    AssignmentStatement,
    DelayStatement,
    ReturnStatement,
    RaiseStatement,
    Expression,
    IfBlockFwd,
    WorkflowDefinitionFwd
>;
// clang-format on

using WorkflowScope = Vector<WorkflowStatement, struct WorkflowScopeTag>;

struct ConditionBlock : Located {
    Expression condition;
    WorkflowScope statements;
};

struct IfBlock : Located {
    ConditionBlock if_block;
    std::vector<ConditionBlock> elif_blocks;
    WorkflowScope else_block;
};

struct WorkflowDefinition : Located {
    Identifier name;
    Option<Signature> signature;
    WorkflowScope statements;
};

using Program = Vector<WorkflowStatement, struct ProgramTag>;

}  // namespace ez::flow::ast
