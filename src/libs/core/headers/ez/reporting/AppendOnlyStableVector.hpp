#pragma once

#include <ez/Contract.hpp>

#include <atomic>
#include <list>

#include <boost/container/static_vector.hpp>

namespace ez::internal {
/// Append only stable vector.
/// Preserves elements addresses after insertion and keeps track of
/// the element count in an atomic variable.
/// Designed for 1 writer/ multiple readers lock free access.
template <typename T, std::size_t BlockSize = 64>
class AppendOnlyStableVector {
public:
    void append(const T& t)
    {
        maybe_grow();
        m_blocks.back().push_back(t);
        ++m_size;
    }

    void append(T&& t)
    {
        maybe_grow();
        m_blocks.back().push_back(std::move(t));
        ++m_size;
    }

    const T& at(std::size_t i) const
    {
        EZ_ASSERT(m_size);
        EZ_ASSERT(i < m_size);
        const std::size_t blockIndex = i / BlockSize;
        const std::size_t localIndex = i % BlockSize;
        auto iter = m_blocks.begin();
        std::advance(iter, blockIndex);
        return iter->at(localIndex);
    }

    const T& last() const
    {
        EZ_ASSERT(m_size > 0);
        return m_blocks.back().back();
    }

    std::size_t size() const { return m_size; }

    bool empty() const { return m_size == 0; }

    template <typename F>
    void visit(F&& f)
    {
        // This counter is a guard that contains the maximum number of elements to visit, it
        // represents a snapshot of the state of vector at a given time. This is to prevent visiting
        // elements being inserted and not yet published.
        std::size_t count = m_size;

        for (auto& block : m_blocks) {
            for (auto& item : block) {
                if (!count) return;
                std::forward<F>(f)(item);
                count--;
            }
        }
    }

    template <typename F>
    void visit(F&& f) const
    {
        // This counter is a guard that contains the maximum number of elements to visit, it
        // represents a snapshot of the state of vector at a given time. This is to prevent visiting
        // elements being inserted and not yet published.
        std::size_t count = m_size;

        for (auto& block : m_blocks) {
            for (auto& item : block) {
                if (!count) return;
                std::forward<F>(f)(item);
                count--;
            }
        }
    }

    template <typename F>
    void visit_while_true(F&& f) const
    {
        // This counter is a guard that contains the maximum number of elements to visit, it
        // represents a snapshot of the state of vector at a given time. This is to prevent visiting
        // elements being inserted and not yet published.
        std::size_t count = m_size;

        for (auto& block : m_blocks) {
            for (auto& item : block) {
                if (!count) return;
                if (!std::forward<F>(f)(item)) return;
                count--;
            }
        }
    }

private:
    void maybe_grow()
    {
        if (m_size == m_capacity) {
            Block block;
            block.reserve(BlockSize);
            m_blocks.push_back(std::move(block));
            m_capacity += BlockSize;
            ++m_blockCount;
        }
    }

private:
    using Block = boost::container::static_vector<T, BlockSize>;

    std::list<Block> m_blocks;
    std::atomic_size_t m_size{0};
    std::atomic_size_t m_capacity{0};
    std::atomic_size_t m_blockCount{0};
};
}  // namespace ez::internal
