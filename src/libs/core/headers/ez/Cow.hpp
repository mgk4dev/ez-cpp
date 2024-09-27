#pragma once

#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

#include <memory>

namespace ez {
///
/// Cow is a copy on write value wrapper.
/// Usage:
/// @code
/// Cow<std::string> cow{in_place, 4, '='};
/// ASSERT_EQ(cow.value(), "====");
/// cow = "toto";
/// ASSERT_EQ(cow.use_count(), 1);
/// auto cow2 = cow;
/// ASSERT_EQ(cow.use_count(), 2);
/// @endcode
///
template <typename T>
class Cow {
public:
    Cow();

    template <typename... Args>
    Cow(Inplace, Args&&... args);
    Cow(trait::DerivedFrom<T> auto&& val);

    Cow(const T& val);
    Cow(T& val);
    Cow(T&& val);

    Cow(Cow&) = default;
    Cow(const Cow&) = default;
    Cow(Cow&&) = default;

    template <trait::DerivedFrom<T> U>
    Cow(const Cow<U>& val);

    template <trait::DerivedFrom<T> U>
    Cow(Cow<U>& val);

    template <trait::DerivedFrom<T> U>
    Cow(Cow<U>&& val);

    ///////////////////////////////////////////////////////////////////////////

    Cow& operator=(Cow&) = default;
    Cow& operator=(const Cow&) = default;
    Cow& operator=(Cow&&) = default;

    template <trait::DerivedFrom<T> U>
    Cow& operator=(const Cow<U>& rhs);

    template <trait::DerivedFrom<T> U>
    Cow& operator=(Cow<U>& rhs);

    template <trait::DerivedFrom<T> U>
    Cow& operator=(Cow<U>&& rhs);

    bool operator==(const T& val) const;
    bool operator==(const Cow<T>& val) const;

    ///////////////////////////////////////////////////////////////////////////

    bool is_unique() const;
    std::size_t use_count() const;

    template <typename F>
    decltype(auto) edit(F&& f);

    Cow& operator=(auto&& new_val);

    const T* operator->() const;
    const T& value() const;

    void detach();

private:
    template <typename U>
    friend class Cow;

private:
    std::shared_ptr<T> m_data;
};

template <typename T>
Cow(T&&) -> Cow<std::decay_t<T>>;

template <typename T>
Cow<T>::Cow() : m_data{std::make_shared<T>()}
{
}

template <typename T>
template <typename... Args>
Cow<T>::Cow(Inplace, Args&&... args) : m_data{std::make_shared<T>(EZ_FWD(args)...)}
{
}

template <typename T>
Cow<T>::Cow(trait::DerivedFrom<T> auto&& val)
    : m_data{std::make_shared<EZ_DECAY_T(val)>(EZ_FWD(val))}
{
}

template <typename T>
Cow<T>::Cow(const T& val) : m_data{std::make_shared<T>(val)}
{
}

template <typename T>
Cow<T>::Cow(T& val) : m_data{std::make_shared<T>(val)}
{
}

template <typename T>
Cow<T>::Cow(T&& val) : m_data{std::make_shared<T>(std::move(val))}
{
}

template <typename T>
template <trait::DerivedFrom<T> U>
Cow<T>::Cow(const Cow<U>& val) : m_data{val.m_data}
{
}

template <typename T>
template <trait::DerivedFrom<T> U>
Cow<T>::Cow(Cow<U>& val) : m_data{val.m_data}
{
}

template <typename T>
template <trait::DerivedFrom<T> U>
Cow<T>::Cow(Cow<U>&& val) : m_data{std::move(val.m_data)}
{
}

///////////////////////////////////////////////////////////////////////////

template <typename T>
template <trait::DerivedFrom<T> U>
Cow<T>& Cow<T>::operator=(const Cow<U>& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

template <typename T>
template <trait::DerivedFrom<T> U>
Cow<T>& Cow<T>::operator=(Cow<U>& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

template <typename T>
template <trait::DerivedFrom<T> U>
Cow<T>& Cow<T>::operator=(Cow<U>&& rhs)
{
    m_data = std::move(rhs.m_data);
    return *this;
}

template <typename T>
bool Cow<T>::operator==(const T& val) const
{
    return value() == val;
}

template <typename T>
bool Cow<T>::operator==(const Cow<T>& val) const
{
    return value() == val.value();
}

///////////////////////////////////////////////////////////////////////////

template <typename T>
bool Cow<T>::is_unique() const
{
    return m_data.use_count() == 1;
}

template <typename T>
std::size_t Cow<T>::use_count() const
{
    return m_data.use_count();
}

template <typename T>
template <typename F>
decltype(auto) Cow<T>::edit(F&& f)
{
    if (!is_unique()) detach();
    f(*m_data);
}

template <typename T>
Cow<T>& Cow<T>::operator=(auto&& new_val)
{
    edit([&](T& val) { val = EZ_FWD(new_val); });
    return *this;
}

template <typename T>
const T* Cow<T>::operator->() const
{
    return m_data.get();
}

template <typename T>
const T& Cow<T>::value() const
{
    return *m_data;
}

template <typename T>
void Cow<T>::detach()
{
    m_data = std::make_shared<T>(*m_data);
}

}  // namespace ez
