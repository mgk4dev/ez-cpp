#pragma once

#include <ez/trait.hpp>
#include <ez/utils.hpp>

#include <memory>

namespace ez {

template <typename T>
class Cow {
public:
    Cow() : m_data{std::make_shared<T>()} {}

    template <typename... Args>
    Cow(Inplace, Args&&... args) : m_data{std::make_shared<T>(EZ_FWD(args)...)}
    {
    }

    Cow(trait::DerivedFrom<T> auto&& val) : m_data{std::make_shared<EZ_DECAY_T(val)>(EZ_FWD(val))}

    {
    }

    Cow(const T& val) : m_data{std::make_shared<T>(val)} {}
    Cow(T& val) : m_data{std::make_shared<T>(val)} {}
    Cow(T&& val) : m_data{std::make_shared<T>(std::move(val))} {}

    Cow(Cow&) = default;
    Cow(const Cow&) = default;
    Cow(Cow&&) = default;

    template <trait::DerivedFrom<T> U>
    Cow(const Cow<U>& val) : m_data{val.m_data}
    {
    }

    template <trait::DerivedFrom<T> U>
    Cow(Cow<U>& val) : m_data{val.m_data}
    {
    }

    template <trait::DerivedFrom<T> U>
    Cow(Cow<U>&& val) : m_data{std::move(val.m_data)}
    {
    }

    ///////////////////////////////////////////////////////////////////////////

    Cow& operator=(Cow&) = default;
    Cow& operator=(const Cow&) = default;
    Cow& operator=(Cow&&) = default;

    template <trait::DerivedFrom<T> U>
    Cow& operator=(const Cow<U>& rhs)
    {
        m_data = rhs.m_data;
        return *this;
    }

    template <trait::DerivedFrom<T> U>
    Cow& operator=(Cow<U>& rhs)
    {
        m_data = rhs.m_data;
        return *this;
    }

    template <trait::DerivedFrom<T> U>
    Cow& operator=(Cow<U>&& rhs)
    {
        m_data = std::move(rhs.m_data);
        return *this;
    }

    bool operator==(const T& val) const { return value() == val; }
    bool operator==(const Cow<T>& val) const { return value() == val.value(); }

    ///////////////////////////////////////////////////////////////////////////

    bool is_unique() const { return m_data.unique(); }
    std::size_t use_count() const { return m_data.use_count(); }

    template <typename F>
    decltype(auto) edit(F&& f)
    {
        if (!is_unique()) detach();
        f(*m_data);
    }

    Cow& operator=(auto&& new_val)
    {
        edit([&](T& val) { val = EZ_FWD(new_val); });
        return *this;
    }

    const T* operator->() const { return m_data.get(); }
    const T& value() const { return *m_data; }

    void detach() { m_data = std::make_shared<T>(*m_data); }

private:
    template <typename U>
    friend class Cow;

private:
    std::shared_ptr<T> m_data;
};

template <typename T>
Cow(T&&) -> Cow<std::decay_t<T>>;
}  // namespace ez
