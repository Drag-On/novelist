/**********************************************************
 * @file   Identity.h
 * @author jan
 * @date   7/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_IDENTITY_H
#define NOVELIST_IDENTITY_H

#include <cstddef>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <novelist_core_export.h>

namespace novelist {

    /**
     * Exception thrown when an ID is requested that isn't available anymore
     */
    class NOVELIST_CORE_EXPORT uniqueness_error : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };

    template<typename Tag_Type, typename T>
    class IdManager;

    template<typename Tag_Type, typename T>
    class Id;

    namespace internal {
        template<typename Tag_Type, typename T>
        bool isValidId(Id<Tag_Type, T> const& id);
    }

    /**
     * Represents a unique identifier
     * @tparam Tag_Type IDs are unique among all IDs with the same tag
     * @tparam T Underlying integral type
     * @note This is not thread-safe.
     */
    template<typename Tag_Type, typename T>
    class Id {
    public:
        Id() = delete;

        /**
         * Returns the ID to the pool of available IDs
         */
        ~Id() noexcept
        {
            if (m_mgr != nullptr)
                m_mgr->reposit(m_id);
        }

        Id(Id<Tag_Type, T> const& other) = delete;

        Id<Tag_Type, T>& operator=(Id<Tag_Type, T> const& other) = delete;

        Id(Id<Tag_Type, T>&& other) noexcept
                :m_mgr(other.m_mgr),
                 m_id(other.m_id)
        {
            other.m_mgr = nullptr;
        }

        Id<Tag_Type, T>& operator=(Id<Tag_Type, T>&& other) noexcept
        {
            if (this != &other) {
                if (m_mgr != nullptr)
                    m_mgr->reposit(m_id);

                m_mgr = other.m_mgr;
                m_id = other.m_id;
                other.m_mgr = nullptr;
            }
            return *this;
        }

        /**
         * Equality operator
         * @param other Other identifier
         * @return True if \p other is a reference to this, otherwise false. If either object is in a moved-from state,
         *         then this returns false, even in the case of self-test.
         */
        bool operator==(Id<Tag_Type, T> const& other) const noexcept
        {
            return other.m_id == m_id && m_mgr != nullptr && other.m_mgr != nullptr;
        }

        /**
         * Inequality operator
         * @param other Other identifier
         * @return True if \p other is not a reference to this, otherwise false
         */
        bool operator!=(Id<Tag_Type, T> const& other) const noexcept
        {
            return !(*this == other);
        }

        /**
         * Write ID to stream in human-readable format
         * @param stream Stream to write to
         * @param id ID to wirte
         * @return The stream
         */
        friend std::ostream& operator<<(std::ostream& stream, Id<Tag_Type, T> const& id)
        {
            auto curFill = stream.fill();
            stream << std::setw(static_cast<int>(std::to_string(std::numeric_limits<T>::max()).size()))
                   << std::setfill('0') << id.m_id << std::setfill(curFill);
            return stream;
        }

        /**
         * @return String representation of the ID
         */
        std::string toString() const
        {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        /**
         * @return The underlying ID
         */
        T id() const noexcept
        {
            return m_id;
        }

    private:
        IdManager<Tag_Type, T>* m_mgr;
        T m_id;

        Id(IdManager<Tag_Type, T>* mgr, T id) noexcept
                :m_mgr(mgr),
                 m_id(id)
        {
        }

        friend class IdManager<Tag_Type, T>;

        friend bool internal::isValidId<>(Id<Tag_Type, T> const&);
    };

    /**
     * Manages unique identifiers
     * @tparam Tag_Type IDs are unique among all IDs with the same tag
     * @tparam T Underlying integral type. Defaults to uint32_t, which allows up to 4,294,967,295 unique IDs.
     * @note This is not thread-safe.
     */
    template<typename Tag_Type, typename T = uint32_t>
    class IdManager {
    public:
        /**
         * Type of generated IDs
         */
        using IdType = Id<Tag_Type, T>;

        IdManager() noexcept
        {
            static_assert(std::is_integral_v<T>, "Base type must be integral");
        }

        ~IdManager() noexcept = default;

        IdManager(IdManager<Tag_Type, T> const& other) = delete;

        IdManager<Tag_Type, T>& operator=(IdManager<Tag_Type, T> const& other) = delete;

        IdManager(IdManager<Tag_Type, T>&& other) noexcept = delete;

        IdManager<Tag_Type, T>& operator=(IdManager<Tag_Type, T>&& other) noexcept = delete;

        /**
         * @return A new valid unique ID
         */
        Id<Tag_Type, T> generate();

        /**
         * Allows to request the ID with a particular number.
         * @param id ID to request
         * @return The requested ID
         * @throws uniqueness_error if the requested ID is not available anymore
         */
        Id<Tag_Type, T> request(T id);

    private:
        T m_next = std::numeric_limits<T>::min();
        std::vector<T> m_freeList;

        void reposit(T id);

        void checkFreeList();

        friend class Id<Tag_Type, T>;

        friend bool internal::isValidId<>(Id<Tag_Type, T> const&);
    };

    template<typename Tag_Type, typename T>
    Id<Tag_Type, T> IdManager<Tag_Type, T>::generate()
    {
        if (m_freeList.empty())
            return Id<Tag_Type, T>(this, m_next++);

        Id<Tag_Type, T> id(this, m_freeList.back());
        m_freeList.pop_back();
        checkFreeList();
        return id;
    }

    template<typename Tag_Type, typename T>
    Id<Tag_Type, T> IdManager<Tag_Type, T>::request(T id)
    {
        // If the requested ID is beyond the current next value, then we need to fast-forward
        if (id >= m_next) {
            for (T i = m_next; i < id; ++i)
                m_freeList.push_back(i);
            Id<Tag_Type, T> genId(this, id);
            m_next = id + 1;
            checkFreeList();
            return genId;
        }

        // Otherwise, check whether the ID is free, then create it
        for (auto iter = m_freeList.begin(); iter != m_freeList.end(); ++iter) {
            if (*iter == id) {
                m_freeList.erase(iter);
                return Id<Tag_Type, T>(this, id);
            }
        }

        throw uniqueness_error{"The requested id " + std::to_string(id) + " was already taken."};
    }

    template<typename Tag_Type, typename T>
    void IdManager<Tag_Type, T>::reposit(T id)
    {
        m_freeList.push_back(id);
        checkFreeList();
    }

    template<typename Tag_Type, typename T>
    void IdManager<Tag_Type, T>::checkFreeList()
    {
        std::sort(m_freeList.begin(), m_freeList.end());
        while (!m_freeList.empty() && m_freeList.back() == m_next - 1) {
            --m_next;
            m_freeList.pop_back();
        }
    }

    namespace internal {
        template<typename Tag_Type, typename T>
        bool isValidId(Id<Tag_Type, T> const& id)
        {
            return id.m_mgr != nullptr &&
                    id.m_id < id.m_mgr->m_next &&
                    std::find(id.m_mgr->m_freeList.begin(), id.m_mgr->m_freeList.end(), id.m_id)
                            == id.m_mgr->m_freeList.end();
        }
    }
}

#endif //NOVELIST_IDENTITY_H
