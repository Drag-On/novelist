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

namespace novelist {

    class uniqueness_error : public std::logic_error {
    public:
        explicit uniqueness_error(const std::string& what_arg)
                :std::logic_error(what_arg)
        {
        }

        explicit uniqueness_error(const char* what_arg)
                :std::logic_error(what_arg)
        {
        }
    };

    template<typename Tag_Type, typename T>
    class IdManager;

    template<typename Tag_Type, typename T>
    class Id {
    public:
        ~Id() noexcept
        {
            m_mgr->reposit(m_id);
        }

        Id(Id<Tag_Type, T> const& other) = delete;

        Id<Tag_Type, T>& operator=(Id<Tag_Type, T> const& other) = delete;

        Id(Id<Tag_Type, T>&& other) noexcept = default;

        Id<Tag_Type, T>& operator=(Id<Tag_Type, T>&& other) noexcept = default;

        bool operator==(Id<Tag_Type, T> const& other) const noexcept
        {
            return other.m_id == m_id;
        }

        bool operator!=(Id<Tag_Type, T> const& other) const noexcept
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& stream, Id<Tag_Type, T> const& id)
        {
            auto curFill = stream.fill();
            stream << std::setw(static_cast<int>(std::to_string(std::numeric_limits<T>::max()).size()))
                   << std::setfill('0') << id.m_id << std::setfill(curFill);
            return stream;
        }

        std::string toString() const
        {
            std::stringstream ss;
            ss << *this;
            return ss.str();
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
    };

    template<typename Tag_Type, typename T = uint32_t>
    class IdManager {
    public:
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

        Id<Tag_Type, T> generate();

        Id<Tag_Type, T> request(T id);

    private:
        T m_next = std::numeric_limits<T>::min();
        std::vector<T> m_freeList;

        void reposit(T id);

        void checkFreeList();

        friend class Id<Tag_Type, T>;
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
            return generate();
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
        if (id == m_next - 1) {
            --m_next;
            checkFreeList();
        }
        else
            m_freeList.push_back(id);
    }

    template<typename Tag_Type, typename T>
    void IdManager<Tag_Type, T>::checkFreeList()
    {
        while (!m_freeList.empty() && m_freeList.back() == m_next - 1) {
            --m_next;
            m_freeList.pop_back();
        }
    }
}

#endif //NOVELIST_IDENTITY_H
