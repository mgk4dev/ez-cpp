#pragma once

#include <ez/Utils.hpp>

#include <shared_mutex>
#include <utility>

namespace ez {
template <typename Mutex>
struct LockHelper {
    static inline void lock(Mutex& m) { m.lock(); }
    static inline void unlock(Mutex& m) { m.unlock(); }
    static inline void lock_shared(Mutex& m) { m.lock_shared(); }
    static inline void unlock_shared(Mutex& m) { m.unlock_shared(); }
};

template <>
struct LockHelper<std::mutex> {
    static inline void lock(std::mutex& m) { m.lock(); }
    static inline void unlock(std::mutex& m) { m.unlock(); }
    static inline void lock_shared(std::mutex& m) { m.lock(); }
    static inline void unlock_shared(std::mutex& m) { m.unlock(); }
};

///
/// Atomic is a utility type to protect the access to an object
/// in read/write mode across multiple threads.
/// Usage:
///   @code
///   Atomic<std::vector<int>> vec;
///   vec->push_back(22); // unique lock then push_back then unlock
///   auto size = std::as_const(vec)->size() ; // shared lock then size() then unlock
///   vec.edit([](std::vector<int>& vec)
///   {
///      vec.push_back(33);
///   });
///   @endcode
template <typename T, typename Mutex = std::shared_mutex>
class Atomic {
    T m_data;
    mutable Mutex m_mutex;

public:
    template <typename TA, void(lock_fn)(Mutex&), void(unlock_fn)(Mutex&), bool is_shared_val>
    class LockGuard {
        Atomic* m_atomic = nullptr;

    public:
        LockGuard(Atomic* d) : m_atomic(d) { lock(); }
        ~LockGuard() { unlock(); }

        LockGuard(LockGuard&& rhs) { std::swap(m_atomic, rhs.m_atomic); }

        LockGuard& operator=(LockGuard&& rhs)
        {
            std::swap(m_atomic, rhs.m_atomic);
            return *this;
        }

        LockGuard& operator=(const LockGuard& rhs) = delete;
        LockGuard(const LockGuard& rhs) = delete;

        TA* operator->() { return &m_atomic->m_data; }

        static constexpr bool is_shared() { return is_shared_val; }
        static constexpr bool is_unique() { return !is_shared_val; }

    private:
        void lock()
        {
            if (m_atomic) lock_fn(m_atomic->m_mutex);
        }
        void unlock()
        {
            if (m_atomic) unlock_fn(m_atomic->m_mutex);
        }
    };

    using SharedLockGuard =
        LockGuard<const T, LockHelper<Mutex>::lock_shared, LockHelper<Mutex>::unlock_shared, true>;
    using ExclusiveLockGuard =
        LockGuard<T, LockHelper<Mutex>::lock, LockHelper<Mutex>::unlock, false>;

    //////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename... Args>
    Atomic(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>);

    Atomic(Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>);

    Atomic(const Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>);

    Atomic(Atomic&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>);

    Atomic& operator=(Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>);

    Atomic& operator=(const Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>);

    Atomic& operator=(Atomic&& rhs) noexcept(std::is_nothrow_move_assignable_v<T>);

    SharedLockGuard lock_shared() const;

    ExclusiveLockGuard lock_unique();

    template <typename F>
    auto read(F&& f) const -> decltype(f(m_data));

    template <typename F>
    auto edit(F&& f) -> decltype(f(m_data));

    Atomic& operator=(auto&& val);

    SharedLockGuard operator->() const;
    ExclusiveLockGuard operator->();
};

template <typename T, typename M>
template <typename... Args>
Atomic<T, M>::Atomic(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    : m_data(EZ_FWD(args)...)
{
}

template <typename T, typename M>
Atomic<T, M>::Atomic(Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>)
{
    auto _ = rhs.lock_shared();
    m_data = rhs.m_data;
}

template <typename T, typename M>
Atomic<T, M>::Atomic(const Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>)
{
    auto _ = rhs.lock_shared();
    m_data = rhs.m_data;
}

template <typename T, typename M>
Atomic<T, M>::Atomic(Atomic&& rhs) noexcept(std::is_nothrow_move_constructible_v<T>)
    : m_data(std::move(rhs.m_data))
{
}

template <typename T, typename M>
Atomic<T, M>& Atomic<T, M>::operator=(Atomic& rhs) noexcept(std::is_nothrow_copy_assignable_v<T>)
{
    return *this = std::as_const(rhs);
}

template <typename T, typename M>
Atomic<T, M>& Atomic<T, M>::operator=(const Atomic& rhs) noexcept(
    std::is_nothrow_copy_assignable_v<T>)
{
    if (this < &rhs) {
        auto _ = lock_unique();
        auto __ = rhs.lock_shared();

        m_data = rhs.m_data;
    }
    else if (this > &rhs) {
        auto _ = rhs.lock_shared();
        auto __ = lock_unique();
        m_data = rhs.m_data;
    }
    return *this;
}

template <typename T, typename M>
Atomic<T, M>& Atomic<T, M>::operator=(Atomic&& rhs) noexcept(std::is_nothrow_move_assignable_v<T>)
{
    auto _ = lock_unique();
    m_data = std::move(rhs.m_data);
    return *this;
}

template <typename T, typename M>
Atomic<T, M>::SharedLockGuard Atomic<T, M>::lock_shared() const
{
    return SharedLockGuard(const_cast<Atomic*>(this));
}

template <typename T, typename M>
Atomic<T, M>::ExclusiveLockGuard Atomic<T, M>::lock_unique()
{
    return ExclusiveLockGuard(this);
}

template <typename T, typename M>
template <typename F>
auto Atomic<T, M>::read(F&& f) const -> decltype(f(m_data))
{
    auto lock = this->lock_shared();
    return f(m_data);
}

template <typename T, typename M>
template <typename F>
auto Atomic<T, M>::edit(F&& f) -> decltype(f(m_data))
{
    auto lock = this->lock_unique();
    return f(m_data);
}

template <typename T, typename M>
Atomic<T, M>& Atomic<T, M>::operator=(auto&& val)
{
    edit([&](auto& self) { self = EZ_FWD(val); });
    return *this;
}

template <typename T, typename M>
Atomic<T, M>::SharedLockGuard Atomic<T, M>::operator->() const
{
    return lock_shared();
}

template <typename T, typename M>
Atomic<T, M>::ExclusiveLockGuard Atomic<T, M>::operator->()
{
    return lock_unique();
}

}  // namespace ez
