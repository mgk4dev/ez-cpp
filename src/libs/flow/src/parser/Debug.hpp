#pragma once

#define BOOST_SPIRIT_X3_DEBUG

#include "AstAdapted.hpp"

#include <ostream>

namespace ez::flow::ast {
template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Identifier& val)
{
    stream << std::string_view{val};
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Option<T>& val)
{
    if (val)
        stream << val;
    else
        stream << "none";

    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, DurationUnit unit)
{
    return stream << "Unit " << std::to_underlying(unit);
}

inline std::ostream& operator<<(std::ostream& stream, MultiplicativeOperator op)
{
    return stream << "MultiplicativeOperator " << std::to_underlying(op);
}

inline std::ostream& operator<<(std::ostream& stream, AdditiveOperator op)
{
    return stream << "AdditiveOperator " << std::to_underlying(op);
}

inline std::ostream& operator<<(std::ostream& stream, RelationalOperator op)
{
    return stream << "RelationalOperator " << std::to_underlying(op);
}

inline std::ostream& operator<<(std::ostream& stream, LogicalOperator op)
{
    return stream << "LogicalOperator " << std::to_underlying(op);
}

inline std::ostream& operator<<(std::ostream& stream, EqualityOperator op)
{
    return stream << "EqualityOperator " << std::to_underlying(op);
}

inline std::ostream& operator<<(std::ostream& stream, const WorkflowDefinition&)
{
    return stream << "Workflow";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Forward<T>& fwd)
{
    return stream << fwd.get();
}

inline std::ostream& operator<<(std::ostream& stream, const FunctionCall&)
{
    return stream << "FunctionCall";
}

inline std::ostream& operator<<(std::ostream& stream, const Dictionary&)
{
    return stream << "Dictionary";
}

inline std::ostream& operator<<(std::ostream& stream, const Array&) { return stream << "Array"; }

inline std::ostream& operator<<(std::ostream& stream, const Expression&)
{
    return stream << "Expression";
}

inline std::ostream& operator<<(std::ostream& stream, const ConditionBlock&)
{
    return stream << "ConditionBlock ";
}

inline std::ostream& operator<<(std::ostream& stream, const IfBlock&)
{
    return stream << "IfBlock ";
}

inline std::ostream& operator<<(std::ostream& stream, const BreakStatement&)
{
    return stream << "BreakStatement ";
}

inline std::ostream& operator<<(std::ostream& stream, const RepeatBlock&)
{
    return stream << "RepeatBlock ";
}

inline std::ostream& operator<<(std::ostream& stream, const AwaitExpression&)
{
    return stream << "AwaitExpression ";
}

inline std::ostream& operator<<(std::ostream& stream, const TryExpression&)
{
    return stream << "TryExpression ";
}

}  // namespace ez::flow::ast
