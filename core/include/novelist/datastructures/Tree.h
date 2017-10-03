/**********************************************************
 * @file   tree.h
 * @author jan
 * @date   7/11/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TREE_H
#define NOVELIST_TREE_H

#include <vector>
#include <iostream>
#include <deque>
#include <optional>
#include <gsl/gsl>

namespace novelist {

    /**
     * Node in a tree
     * @tparam T Type of the payload of a node. Must be moveable or copyable (if clone() is used).
     */
    template<typename T>
    class TreeNode {
    private:
        using NodeType = TreeNode<T>;
        using Children = std::vector<TreeNode<T>>;

        TreeNode* m_parent = nullptr;
        Children m_children;

    public:
        using iterator = typename Children::iterator;
        using const_iterator = typename Children::const_iterator;

        /**
         * Payload
         */
        T m_data;

        /**
         * Construct node holding some data
         * @param data Data to encapsulate
         */
        explicit TreeNode(T data) noexcept
                :m_data(std::move(data))
        {
        }

        ~TreeNode() noexcept = default;

        TreeNode(TreeNode const&) = delete;

        TreeNode& operator=(TreeNode const&) = delete;

        TreeNode(TreeNode&&) noexcept = default;

        TreeNode& operator=(TreeNode&&) noexcept = default;

        /**
         * @return Pointer to parent node, or nullptr if this is a root
         */
        TreeNode<T>* parent() const
        {
            return m_parent;
        }

        /**
         * @return Iterator at begin of all direct children
         */
        iterator begin() noexcept
        {
            return m_children.begin();
        }

        /**
         * @return Iterator at begin of all direct children
         */
        const_iterator begin() const noexcept
        {
            return m_children.begin();
        }

        /**
         * @return Iterator at end of all direct children
         */
        iterator end() noexcept
        {
            return m_children.end();
        }

        /**
         * @return Iterator at end of all direct children
         */
        const_iterator end() const noexcept
        {
            return m_children.end();
        }

        /**
         * @return Number of direct children
         */
        size_t size() const noexcept
        {
            return m_children.size();
        }

        /**
         * @return true in case there are any children, otherwise false
         */
        bool empty() const noexcept
        {
            return m_children.empty();
        }

        /**
         * Create a new child
         * @param pos Position of insertion
         * @param data Payload
         * @return Iterator to inserted child
         */
        iterator emplace(const_iterator pos, T data)
        {
            auto iter = m_children.emplace(pos, std::move(data));
            iter->m_parent = this;
            // All children might have been relocated in memory. This invalidates all pointers to them, in particular
            // the parent pointers of their children. This is fixed here.
            for (auto& c : m_children) {
                for (auto& cc : c.m_children)
                    cc.m_parent = &c;
            }
            return iter;
        }

        /**
         * Create new child at the end
         * @param data Payload
         * @return Reference to created element
         */
        NodeType& emplace_back(T data)
        {
            auto old_cap = m_children.capacity();
            auto& node = m_children.emplace_back(std::move(data));
            node.m_parent = this;
            if (old_cap != m_children.capacity()) {
                // All children have been relocated in memory. This invalidates all pointers to them, in particular
                // the parent pointers of their children. This is fixed here.
                for (auto& c : m_children) {
                    for (auto& cc : c.m_children)
                        cc.m_parent = &c;
                }
            }
            return node;
        }

        /**
         * Insert an existing node as child
         * @param pos Position of insertion
         * @param node Node, must not have a parent
         * @return Iterator to inserted child
         */
        iterator insert(const_iterator pos, NodeType node)
        {
            Expects(node.parent() == nullptr);
            Expects(pos >= begin());
            Expects(pos <= end());

            auto iter = m_children.insert(pos, std::move(node));
            iter->m_parent = this;
            // All children might have been relocated in memory. This invalidates all pointers to them, in particular
            // the parent pointers of their children. This is fixed here.
            for (auto& c : m_children) {
                for (auto& cc : c.m_children)
                    cc.m_parent = &c;
            }
            return iter;
        }

        /**
         * Erase child
         * @param pos Position of child to erase
         * @return Iterator to element after the erased one
         */
        iterator erase(const_iterator pos)
        {
            return erase(pos, pos + 1);
        }

        /**
         * Erase range of children
         * @param first Initial position of range
         * @param last Iterator of element after last child to erase
         * @return Iterator to element after the erased ones
         */
        iterator erase(const_iterator first, const_iterator last)
        {
            auto iter = m_children.erase(first, last);
            // All children past the erased might have been relocated in memory. This invalidates all pointers to them,
            // in particular the parent pointers of their children. This is fixed here.
            for(auto cIter = iter; cIter != m_children.end(); ++cIter)
            {
                for(auto& cc : cIter->m_children)
                    cc.m_parent = &*cIter;
            }
            return iter;
        }

        /**
         * Remove child from this node and return it
         * @param pos Position of child
         * @return Node at \p pos
         */
        NodeType take(iterator pos)
        {
            NodeType n = std::move(*pos);
            n.m_parent = nullptr;
            erase(pos);
            return n;
        }

        /**
         * Removes a child from this node and inserts it at a different node
         * @details Destination may not be in the subtree of source
         * @param srcChild Index of the child to move
         * @param destParent Parent node to move to
         * @param destChild Index of the child to insert at
         * @return iterator to the inserted element or destParent.end() in case of failure
         */
        iterator move(size_t srcChild, NodeType& destParent, size_t destChild)
        {
            Expects(srcChild < size());
            Expects(destChild <= destParent.size());

            if (at(srcChild) == destParent || destParent.inSubtreeOf(at(srcChild)))
                return destParent.end();

            if (this == &destParent) {
                if (srcChild == destChild || destChild == srcChild + 1)
                    return begin() + srcChild; // This is a no-op

                if(srcChild < destChild)
                    --destChild;

                NodeType n = take(begin() + srcChild);
                auto const iter = destParent.insert(destParent.begin() + destChild, std::move(n));
                return iter;
            }

            if (destParent.parent() == this)
            {
                auto const destParentIdx = destParent.parentIndex().value();
                size_t const newDestParentIdx = srcChild < destParentIdx ? destParentIdx - 1 : destParentIdx;

                NodeType n = take(begin() + srcChild);

                NodeType& newDestParent = this->at(newDestParentIdx);
                auto const it = newDestParent.insert(newDestParent.begin() + destChild, std::move(n));
                return it;
            }

            NodeType n = take(begin() + srcChild);
            auto const it = destParent.insert(destParent.begin() + destChild, std::move(n));
            return it;
        }

        /**
         * True in case this is part of the tree spanned by \p other
         * @param other Potential parent node
         * @return true in case this is below \p node, otherwise false
         */
        bool inSubtreeOf(TreeNode<T> const& other) const
        {
            for (TreeNode<T> const* parent = m_parent; parent != nullptr; parent = parent->m_parent) {
                if (parent == &other)
                    return true;
            }
            return false;
        }

        /**
         * Provides a copy of this node and all children, but it won't have a parent.
         * @return Deep copy of this node
         */
        NodeType clone()
        {
            NodeType clone{m_data};
            auto setParPtr = [&](auto&& self, NodeType* par, Children const& children) -> void {
                for (auto const& c : children) {
                    auto& n = par->emplace_back(c.m_data);
                    self(self, &n, c.m_children);
                }
            };
            setParPtr(setParPtr, &clone, m_children);
            return clone;
        }

        /**
         * Erase all children
         */
        void clear() noexcept
        {
            m_children.clear();
        }

        /**
         * @param pos Position of child
         * @return Child at \p pos
         */
        NodeType const& operator[](size_t pos) const
        {
            return m_children[pos];
        }

        /**
         * @param pos Position of child
         * @return Child at \p pos
         */
        NodeType& operator[](size_t pos)
        {
            return m_children[pos];
        }

        /**
         * Access child at a position with bounds checking
         * @param pos Position of child
         * @return Child at \p pos
         */
        NodeType const& at(size_t pos) const
        {
            return m_children.at(pos);
        }

        /**
         * Access child at a position with bounds checking
         * @param pos Position of child
         * @return Child at \p pos
         */
        NodeType& at(size_t pos)
        {
            return m_children.at(pos);
        }

        /**
         * @return The index of this node within its parent's list of children, if it has a parent
         */
        std::optional<size_t> parentIndex() const
        {
            std::optional<size_t> parIdx{};
            if (parent()) {
                auto iter = std::find(parent()->begin(), parent()->end(), *this);
                assert (iter != parent()->end());
                parIdx = gsl::narrow_cast<size_t>(std::distance(parent()->begin(), iter));
                return parIdx;
            }
            return parIdx;
        }

        /**
         * @param other Other node
         * @return true in case both nodes are identical, ie. not only equivalent, but the same object
         */
        bool operator==(NodeType const& other) const
        {
            return this == &other;
        }

        /**
         * @param other Other node
         * @return true in case both nodes are different, otherwise false
         */
        bool operator!=(NodeType const& other) const
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& stream, NodeType const& node)
        {
            auto print = [&](auto& self, NodeType const& n, size_t indent) -> void
            {
                for(auto const& c : n)
                {
                    for(size_t i = 0; i < indent; ++i)
                        stream << "  ";
                    stream << c.m_data << "\n";
                    self(self, c, indent + 1);
                }
            };

            print(print, node, 0);
            return stream;
        }
    };

    /**
     * Traverse a tree using depth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C1 Type of callback function
     * @tparam C2 Type of second callback function
     * @param root Node to start search at
     * @param callback Called on every node in a depth first order. Must return true to break search.
     * @param after Called after all children of a node have been visited
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C1,
            typename C2,
            typename = std::enable_if_t<
                    std::is_invocable_r_v<bool, C1, TreeNode<T>&> && std::is_invocable_v<C2, TreeNode<T>&>, int>>
    TreeNode<T>* traverse_dfs(TreeNode<T>& root, C1 callback, C2 after)
    {
        if (callback(root))
            return &root;

        TreeNode<T>* found = nullptr;
        for (auto& n : root) {
            if ((found = traverse_dfs(n, callback, after)) != nullptr)
                break;
        }

        after(root);

        return found;
    }

    /**
     * Traverse a tree using depth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C Type of callback function
     * @param root Node to start search at
     * @param callback Called on every node in a depth first order. Must return true to break search.
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, C, TreeNode<T>&>, int>>
    TreeNode<T>* traverse_dfs(TreeNode<T>& root, C callback)
    {
        auto noop = [](TreeNode<T>&) { };
        return traverse_dfs(root, callback, noop);
    }

    /**
     * Traverse a tree using depth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C1 Type of callback function
     * @tparam C2 Type of second callback function
     * @param root Node to start search at
     * @param callback Called on every node in a depth first order. Must return true to break search.
     * @param after Called after all children of a node have been visited
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C1,
            typename C2,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, C1, TreeNode<T> const&>
                    && std::is_invocable_v<C2, TreeNode<T> const&>, int>>
    TreeNode<T> const* traverse_dfs(TreeNode<T> const& root, C1 callback, C2 after)
    {
        return traverse_dfs(const_cast<TreeNode<T>&>(root),
                [&](TreeNode<T>& n) { return callback(const_cast<TreeNode<T> const&>(n)); },
                [&](TreeNode<T>& n) { after(const_cast<TreeNode<T> const&>(n)); });
    }

    /**
     * Traverse a tree using depth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C Type of callback function
     * @param root Node to start search at
     * @param callback Called on every node in a depth first order. Must return true to break search.
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, C, TreeNode<T> const&>, int>>
    TreeNode<T> const* traverse_dfs(TreeNode<T> const& root, C callback)
    {
        auto noop = [](TreeNode<T> const&) { };
        return traverse_dfs(root, callback, noop);
    }

    /**
     * Traverse a tree using breadth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C1 Type of callback function
     * @tparam C2 Type of second callback function
     * @param root Node to start search at
     * @param callback Called on every node in a breadth first order. Must return true to break search.
     * @param after Called after all children of a node have been visited
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C1,
            typename C2,
            typename = std::enable_if_t<
                    std::is_invocable_r_v<bool, C1, TreeNode<T>&> && std::is_invocable_v<C2, TreeNode<T>&>, int>>
    TreeNode<T>* traverse_bfs(TreeNode<T>& root, C1 callback, C2 after)
    {
        using NodeType = TreeNode<T>;

        std::deque<NodeType*> q;
        q.push_back(&root);
        while (!q.empty()) {
            auto n = q.front();
            q.pop_front();
            if (callback(*n))
                return n;
            for (auto& elem : *n)
                q.push_back(&elem);
            after(*n);
        }

        return nullptr;
    }

    /**
     * Traverse a tree using breadth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C1 Type of callback function
     * @tparam C2 Type of second callback function
     * @param root Node to start search at
     * @param callback Called on every node in a breadth first order. Must return true to break search.
     * @param after Called after all children of a node have been visited
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, C, TreeNode<T>&>, int>>
    TreeNode<T>* traverse_bfs(TreeNode<T>& root, C callback)
    {
        auto noop = [](TreeNode<T>&) { };
        return traverse_bfs(root, callback, noop);
    }

    /**
     * Traverse a tree using breadth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C1 Type of callback function
     * @tparam C2 Type of second callback function
     * @param root Node to start search at
     * @param callback Called on every node in a breadth first order. Must return true to break search.
     * @param after Called after all children of a node have been visited
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C1,
            typename C2,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, C1, TreeNode<T> const&> &&
                    std::is_invocable_v<C2, TreeNode<T> const&>, int>>
    TreeNode<T> const* traverse_bfs(TreeNode<T> const& root, C1 callback, C2 after)
    {
        return traverse_bfs(const_cast<TreeNode<T>&>(root),
                [&](TreeNode<T>& n) { return callback(const_cast<TreeNode<T> const&>(n)); },
                [&](TreeNode<T>& n) { after(const_cast<TreeNode<T> const&>(n)); });
    }

    /**
     * Traverse a tree using breadth first strategy
     * @note The tree structure may not be modified during the search
     * @tparam T Payload type
     * @tparam C1 Type of callback function
     * @tparam C2 Type of second callback function
     * @param root Node to start search at
     * @param callback Called on every node in a breadth first order. Must return true to break search.
     * @param after Called after all children of a node have been visited
     * @return Pointer to the found element or nullptr, if nothing found
     */
    template<typename T,
            typename C,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, C, TreeNode<T> const&>, int>>
    TreeNode<T> const* traverse_bfs(TreeNode<T> const& root, C callback)
    {
        auto noop = [](TreeNode<T> const&) { };
        return traverse_bfs(root, callback, noop);
    }
}

#endif //NOVELIST_TREE_H
