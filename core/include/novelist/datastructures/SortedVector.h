/**********************************************************
 * @file   SortedVector.h
 * @author jan
 * @date   10/26/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SORTEDVECTOR_H
#define NOVELIST_SORTEDVECTOR_H

#include <functional>

namespace novelist {
    /**
     * Move a range of elements to another position within the same collection.
     * @tparam Iter Iterator type
     * @param src First element to move
     * @param length Amount of elements after \p src to move
     * @param dest Destination. The source element will be inserted before this.
     * @return Iterator to the moved element after the operation
     */
    template<typename Iter>
    Iter move_range(Iter src, size_t length, Iter dest)
    {
        if (src == dest || src + 1 == dest)
            return src;
        if (src < dest) {
            auto first = src;
            auto middle = first + length;
            auto last = dest;
            return std::rotate(first, middle, last);
        }
        else {
            auto first = std::make_reverse_iterator(src + 1);
            auto middle = first + length;
            auto last = std::make_reverse_iterator(dest);
            return std::rotate(first, middle, last).base() - 1;
        }
    }

    /**
     * A vector that is sorted at all times.
     * @tparam T Base type
     * @tparam Pred Comparison predicate, defaults to std::less
     * @tparam Alloc Allocator, defaults to std::allocator
     */
    template<typename T, typename Pred = std::less<T>, typename Alloc = std::allocator<T>>
    class SortedVector : private std::vector<T, Alloc> {
    private:
        using vector_t = std::vector<T, Alloc>;

    public:
        using const_reference = typename vector_t::const_reference;
        using const_iterator = typename vector_t::const_iterator;
        using const_reverse_iterator = typename vector_t::const_reverse_iterator;

    private:
        const_iterator findInsertIdx(T const& element) const
        {
            Pred comp;
            auto iter = begin();
            for (; iter != end(); ++iter) {
                if (!comp(*iter, element))
                    break;
            }
            return iter;
        }

        const_iterator relocate(const_iterator iter)
        {
            typename vector_t::iterator mutIter = vector_t::begin() + std::distance(begin(), iter);

            Pred comp;
            // Try moving left
            if (mutIter != begin()) {
                auto lIter = std::make_reverse_iterator(mutIter);
                auto lIterCopy = lIter;
                for (; lIter != rend(); std::advance(lIter, 1)) {
                    if (comp(*lIter, *mutIter))
                        break;
                }
                if (lIter != lIterCopy) {
                    auto lIterForward = lIter.base();
                    return move_range(mutIter, 1, lIterForward);
                }
            }
            // Try moving right
            if (mutIter != end()) {
                auto rIter = mutIter;
                std::advance(rIter, 1);
                auto rIterCopy = rIter;
                for (; rIter != end(); std::advance(rIter, 1)) {
                    if (!comp(*rIter, *mutIter))
                        break;
                }
                if (rIter != rIterCopy)
                    return move_range(mutIter, 1, rIter);
            }
            return iter;
        }

    public:
        using vector_t::vector;
        using vector_t::empty;
        using vector_t::size;
        using vector_t::max_size;
        using vector_t::reserve;
        using vector_t::capacity;
        using vector_t::shrink_to_fit;
        using vector_t::clear;
        using vector_t::erase;
        using vector_t::pop_back;
        using vector_t::swap;

        explicit SortedVector(size_t count, Alloc const& alloc = Alloc())
                :vector_t(count, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        template<class InputIt>
        SortedVector(InputIt first, InputIt last, Alloc const& alloc = Alloc())
                : vector_t(first, last, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        explicit SortedVector(vector_t const& other)
                :vector_t(other,
                std::allocator_traits<Alloc>::select_on_container_copy_construction(other.get_allocator()))
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(vector_t const& other, Alloc const& alloc)
                :vector_t(other, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(SortedVector const& other)
                :vector_t(other.begin(), other.end())
        {
        }

        explicit SortedVector(vector_t&& other) noexcept
                :vector_t(std::move(other))
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(vector_t&& other, Alloc const& alloc)
                :vector_t(std::move(other), alloc)
        {
            std::sort(vector_t::begin(), vector_t::end(), Pred());
        }

        SortedVector(SortedVector&& other) noexcept
                :vector_t(std::move(other))
        {
        }

        SortedVector(std::initializer_list<T> init, Alloc const& alloc = Alloc())
                :vector_t(init, alloc)
        {
            std::sort(vector_t::begin(), vector_t::end());
        }

        SortedVector& operator=(SortedVector const& other)
        {
            vector_t::operator=(other);
            return *this;
        }

        SortedVector& operator=(SortedVector&& other) noexcept
        {
            vector_t::operator=(std::move(other));
            return *this;
        }

        SortedVector& operator=(std::initializer_list<T> iList)
        {
            vector_t::operator=(iList);
            std::sort(vector_t::begin(), vector_t::end(), Pred());
            return *this;
        }

        const_reference at(size_t pos) const
        {
            return vector_t::at(pos);
        }

        const_reference operator[](size_t pos) const
        {
            return vector_t::operator[](pos);
        }

        const_reference front() const
        {
            return vector_t::front();
        }

        const_reference back() const
        {
            return vector_t::back();
        }

        const_iterator begin() const
        {
            return vector_t::begin();
        }

        const_iterator end() const
        {
            return vector_t::end();
        }

        const_reverse_iterator rbegin() const
        {
            return vector_t::rbegin();
        }

        const_reverse_iterator rend() const
        {
            return vector_t::rend();
        }

        /**
         * Insert an element into the vector
         * @param value  Value to insert
         * @return Iterator to the inserted element
         */
        const_iterator insert(T const& value)
        {
            return vector_t::insert(findInsertIdx(value), value);
        }

        /**
         * Insert an element into the vector
         * @param value  Value to insert
         * @return Iterator to the inserted element
         */
        const_iterator insert(T&& value)
        {
            return vector_t::insert(findInsertIdx(value), std::move(value));
        }

        /**
         * Insert \p count elements into the vector
         * @param count Amount of copies to insert
         * @param value  Value to insert
         * @return Iterator to the first inserted element
         */
        const_iterator insert(size_t count, T const& value)
        {
            return vector_t::insert(findInsertIdx(value), count, value);
        }

        /**
         * Insert elements from a range of input iterators
         * @tparam InputIt Input iterator type
         * @param first First element
         * @param last Last element
         */
        template<
                typename InputIt,
                typename = std::enable_if<std::is_base_of_v<
                        typename std::iterator_traits<InputIt>::iterator_category, std::input_iterator_tag>>>
        void insert(InputIt first, InputIt last)
        {
            for (auto it = first; it != last; ++it)
                vector_t::insert(findInsertIdx(*it), *it);
        }

        /**
         * Insert from an initializer list
         * @param iList Initializer list to insert
         */
        void insert(std::initializer_list<T> iList)
        {
            insert(iList.begin(), iList.end());
        }

        /**
         * Modify a range of elements. If the sort criterion is affected, the class invariant is restored automatically.
         * @tparam C Function type
         * @param first First element
         * @param last Last element
         * @param functor Functor to modify the elements
         */
        template<typename C, typename = std::enable_if<std::is_invocable_v<void(T&)>>>
        void modify(const_iterator first, const_iterator last, C const& functor)
        {
            for (auto iter = first; iter != last; ++iter) {
                modify(iter, functor);
            }
        }

        /**
         * Modify an element. If the sort criterion is affected, the class invariant is restored automatically.
         * @tparam C Function type
         * @param iter Element to modify
         * @param functor Functor to modify the elements
         * @return Iterator to the modified element
         */
        template<typename C, typename = std::enable_if<std::is_invocable_v<void(T&)>>>
        const_iterator modify(const_iterator iter, C const& functor)
        {
            functor(vector_t::operator[](std::distance(begin(), iter)));
            return relocate(iter);
        }

        friend std::ostream& operator<<(std::ostream& stream, SortedVector const& vec)
        {
            stream << "{ ";
            if (!vec.empty())
                stream << vec.front();
            for (size_t i = 1; i < vec.size(); ++i)
                stream << ", " << vec[i];
            stream << " }";
            return stream;
        }
    };
}

#endif //NOVELIST_SORTEDVECTOR_H
