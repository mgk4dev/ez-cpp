#pragma once

#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

#include <memory>

namespace ez {

template <typename T>
class Shared {
public:
    Shared();

    template <typename... Args>
    Shared(Inplace, Args&&... args);

    Shared(trait::DerivedFrom<T> auto&& val);

    Shared(const T& val);
    Shared(T& val);
    Shared(T&& val);

    Shared(Shared&) = default;
    Shared(const Shared&) = default;
    Shared(Shared&&) = default;

    template <trait::DerivedFrom<T> U>
    Shared(const Shared<U>& val);

    template <trait::DerivedFrom<T> U>
    Shared(Shared<U>& val);

    template <trait::DerivedFrom<T> U>
    Shared(Shared<U>&& val);

    ///////////////////////////////////////////////////////////////////////////

    Shared& operator=(Shared&) = default;
    Shared& operator=(const Shared&) = default;
    Shared& operator=(Shared&&) = default;

    template <trait::DerivedFrom<T> U>
    Shared& operator=(const Shared<U>& rhs);

    template <trait::DerivedFrom<T> U>
    Shared& operator=(Shared<U>& rhs);

    template <trait::DerivedFrom<T> U>
    Shared& operator=(Shared<U>&& rhs);

    Shared& operator=(auto&& new_val);

    bool operator==(const T& val) const;
    bool operator==(const Shared<T>& val) const;

    ///////////////////////////////////////////////////////////////////////////

    bool is_unique() const;
    std::size_t use_count() const;

    const T* operator->() const;
    T* operator->();

    const T& value() const;
    T& value();

    T& operator*() &;
    const T& operator*() const&;

    operator T&() &;
    operator const T&() const&;

    Shared& detach();
    Shared& detach_if_shared();

private:
    template <typename U>
    friend class Shared;

private:
    std::shared_ptr<T> m_data;
};

template <typename T>
Shared(T&&) -> Shared<std::decay_t<T>>;

template <typename T>
Shared<T>::Shared() : m_data{std::make_shared<T>()}
{
}

template <typename T>
template <typename... Args>
Shared<T>::Shared(Inplace, Args&&... args) : m_data{std::make_shared<T>(EZ_FWD(args)...)}
{
}

template <typename T>
Shared<T>::Shared(trait::DerivedFrom<T> auto&& val)
    : m_data{std::make_shared<EZ_DECAY_T(val)>(EZ_FWD(val))}

{
}

template <typename T>
Shared<T>::Shared(const T& val) : m_data{std::make_shared<T>(val)}
{
}
template <typename T>
Shared<T>::Shared(T& val) : m_data{std::make_shared<T>(val)}
{
}
template <typename T>
Shared<T>::Shared(T&& val) : m_data{std::make_shared<T>(std::move(val))}
{
}

template <typename T>
template <trait::DerivedFrom<T> U>
Shared<T>::Shared(const Shared<U>& val) : m_data{val.m_data}
{
}

template <typename T>
template <trait::DerivedFrom<T> U>
Shared<T>::Shared(Shared<U>& val) : m_data{val.m_data}
{
}

template <typename T>
template <trait::DerivedFrom<T> U>
Shared<T>::Shared(Shared<U>&& val) : m_data{std::move(val.m_data)}
{
}

///////////////////////////////////////////////////////////////////////////

template <typename T>
template <trait::DerivedFrom<T> U>
Shared<T>& Shared<T>::operator=(const Shared<U>& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

template <typename T>
template <trait::DerivedFrom<T> U>
Shared<T>& Shared<T>::operator=(Shared<U>& rhs)
{
    m_data = rhs.m_data;
    return *this;
}

template <typename T>
template <trait::DerivedFrom<T> U>
Shared<T>& Shared<T>::operator=(Shared<U>&& rhs)
{
    m_data = std::move(rhs.m_data);
    return *this;
}

template <typename T>
bool Shared<T>::operator==(const T& val) const
{
    return value() == val;
}

template <typename T>
bool Shared<T>::operator==(const Shared<T>& val) const
{
    return value() == val.value();
}

///////////////////////////////////////////////////////////////////////////

template <typename T>
bool Shared<T>::is_unique() const
{
    return use_count() == 1;
}

template <typename T>
std::size_t Shared<T>::use_count() const
{
    return m_data.use_count();
}

template <typename T>
Shared<T>& Shared<T>::operator=(auto&& new_val)
{
    value() = EZ_FWD(new_val);
    return *this;
}

template <typename T>
const T* Shared<T>::operator->() const
{
    return m_data.get();
}

template <typename T>
T* Shared<T>::operator->()
{
    return m_data.get();
}

template <typename T>
const T& Shared<T>::value() const
{
    return *m_data;
}

template <typename T>
T& Shared<T>::value()
{
    return *m_data;
}

template <typename T>
T& Shared<T>::operator*() &
{
    return *m_data;
}

template <typename T>
const T& Shared<T>::operator*() const&
{
    return *m_data;
}

template <typename T>
Shared<T>::operator T&() &
{
    return *m_data;
}

template <typename T>
Shared<T>::operator const T&() const&
{
    return *m_data;
}

template <typename T>
Shared<T>& Shared<T>::detach()
{
    m_data = std::make_shared<T>(*m_data);
    return *this;
}

template <typename T>
Shared<T>& Shared<T>::detach_if_shared()
{
    if (use_count()) detach();
    return *this;
}

}  // namespace ez
