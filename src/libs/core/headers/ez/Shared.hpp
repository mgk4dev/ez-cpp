#pragma once

#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

#include <memory>

namespace ez {

template <typename T>
class Shared {
public:
    Shared() : m_data{std::make_shared<T>()} {}

    template <typename... Args>
    Shared(Inplace, Args&&... args) : m_data{std::make_shared<T>(EZ_FWD(args)...)}
    {
    }

    Shared(trait::DerivedFrom<T> auto&& val)
        : m_data{std::make_shared<EZ_DECAY_T(val)>(EZ_FWD(val))}

    {
    }

    Shared(const T& val) : m_data{std::make_shared<T>(val)} {}
    Shared(T& val) : m_data{std::make_shared<T>(val)} {}
    Shared(T&& val) : m_data{std::make_shared<T>(std::move(val))} {}

    Shared(Shared&) = default;
    Shared(const Shared&) = default;
    Shared(Shared&&) = default;

    template <trait::DerivedFrom<T> U>
    Shared(const Shared<U>& val) : m_data{val.m_data}
    {
    }

    template <trait::DerivedFrom<T> U>
    Shared(Shared<U>& val) : m_data{val.m_data}
    {
    }

    template <trait::DerivedFrom<T> U>
    Shared(Shared<U>&& val) : m_data{std::move(val.m_data)}
    {
    }

    ///////////////////////////////////////////////////////////////////////////

    Shared& operator=(Shared&) = default;
    Shared& operator=(const Shared&) = default;
    Shared& operator=(Shared&&) = default;

    template <trait::DerivedFrom<T> U>
    Shared& operator=(const Shared<U>& rhs)
    {
        m_data = rhs.m_data;
        return *this;
    }

    template <trait::DerivedFrom<T> U>
    Shared& operator=(Shared<U>& rhs)
    {
        m_data = rhs.m_data;
        return *this;
    }

    template <trait::DerivedFrom<T> U>
    Shared& operator=(Shared<U>&& rhs)
    {
        m_data = std::move(rhs.m_data);
        return *this;
    }

    bool operator==(const T& val) const { return value() == val; }
    bool operator==(const Shared<T>& val) const { return value() == val.value(); }

    ///////////////////////////////////////////////////////////////////////////

    bool is_unique() const { return m_data.unique(); }
    std::size_t use_count() const { return m_data.use_count(); }

    Shared& operator=(auto&& new_val)
    {
        value() = EZ_FWD(new_val);
        return *this;
    }

    const T* operator->() const { return m_data.get(); }
    T* operator->() { return m_data.get(); }

    const T& value() const { return *m_data; }
    T& value() { return *m_data; }

    Shared& detach()
    {
        m_data = std::make_shared<T>(*m_data);
        return *this;
    }

    Shared& detach_if_shared()
    {
        if (use_count()) detach();
        return *this;
    }

private:
    template <typename U>
    friend class Shared;

private:
    std::shared_ptr<T> m_data;
};

template <typename T>
Shared(T&&) -> Shared<std::decay_t<T>>;
}  // namespace ez
