/**********************************************************
 * @file   ModelPath.h
 * @author jan
 * @date   9/14/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MODELPATH_H
#define NOVELIST_MODELPATH_H

#include <type_traits>
#include <initializer_list>
#include <vector>
#include <QModelIndex>

namespace novelist {
    /**
     * Index of a column and a row in a model
     */
    using RowColumnIdx = std::pair<int, int>;

    /**
     * Path to a node in a model
     *
     * This does not have to exist, and neither will it track a node's movement (QPersistentModelIndex does that).
     * Instead, an object of this type will always point to the same node position, regardless of whether that element
     * actually exists, has existed before but is now deleted, or if an element on that position has been moved to
     * another location.
     */
    class ModelPath {
    public:
        using iterator = decltype(std::declval<std::vector<RowColumnIdx>>().begin());
        using const_iterator = decltype(std::declval<std::vector<RowColumnIdx>>().cbegin());

        /**
         * Construct empty path (just the invisible root node)
         */
        ModelPath() noexcept = default;

        /**
         * Construct path from a list of rows. Columns are implicitly 0
         * @param rows List of rows, where every element is interpreted as a child of the previous row. The first entry
         * is relative to the invisible root element.
         */
        ModelPath(std::initializer_list<int> rows) noexcept;

        /**
         * Construct path from a list of row/column combinations
         * @param l List of rows and columns, where every element is interpreted as a child of the previous element.
         * The first entry is relative to the invisible root element.
         */
        ModelPath(std::initializer_list<RowColumnIdx> l) noexcept;

        /**
         * Construct path from a range of row/column combinations
         * @param begin First element
         * @param end Element after last element
         */
        ModelPath(const_iterator begin, const_iterator end) noexcept;

        /**
         * Construct path from a QModelIndex
         * @param idx Index to construct path from
         */
        explicit ModelPath(QModelIndex const& idx) noexcept;

        /**
         * Construct a QModelIndex based on the path and a model instance. Might be invalid.
         * @param model Model to construct the QModelIndex for
         * @return The constructed (possibly invalid) index
         */
        QModelIndex toModelIndex(QAbstractItemModel* model) const noexcept;

        /**
         * Checks whether this path is valid on a model.
         * @note If you need the actual model index, use toModelIndex() and check the returned index's isValid()
         * @param model Model to construct the QModelIndex for
         * @return True if the path is valid on \p model, otherwise false
         */
        bool isValid(QAbstractItemModel* model) const noexcept;

        /**
         * @return Depth of the path
         */
        size_t depth() const noexcept;

        /**
         * @return Iterator to the first element of the path
         */
        iterator begin() noexcept;

        /**
         * @return Iterator to the first element of the path
         */
        const_iterator begin() const noexcept;

        /**
         * @return Iterator to the element following the last element of the path
         */
        iterator end() noexcept;

        /**
         * @return Iterator to the element following the last element of the path
         */
        const_iterator end() const noexcept;

        /**
         * @return Path to the second-last entry, i.e. the parent of the element this path points to.
         */
        ModelPath parentPath() const noexcept;

        /**
         * Append another index at the end
         * @param rowColumnIdx New index
         */
        void emplace_back(RowColumnIdx const& rowColumnIdx) noexcept;

        /**
         * Append another index at the end
         * @param row Row index
         */
        void emplace_back(int row) noexcept;

        /**
         * Returns a reference to the path element at specified location.
         * @param pos Position of the element
         * @return Reference to requested element
         */
        RowColumnIdx& operator[](size_t pos);

        /**
         * Returns a constant reference to the path element at specified location.
         * @param pos Position of the element
         * @return Reference to requested element
         */
        RowColumnIdx const& operator[](size_t pos) const;

        /**
         * Reference to leaf element. Calling this on an empty path is undefined.
         */
        RowColumnIdx& leaf();

        /**
         * Constant reference to leaf element. Calling this on an empty path is undefined.
         */
        RowColumnIdx const& leaf() const;

        /**
         * Compares two paths elementwise and determines the index at which they diverge
         * @param other Other path
         * @return Index of the element at which the paths diverge or a number larger than the path's depth if they are equal
         */
        size_t compare(ModelPath const& other) const noexcept;

        /**
         * Compares two paths for equality
         * @param other Path to compare to
         * @return true in case both paths are equivalent, otherwise false
         */
        bool operator==(ModelPath const& other) const noexcept;

        /**
         * Compares two paths for equality
         * @param other Path to compare to
         * @return true in case both paths are different, otherwise false
         */
        bool operator!=(ModelPath const& other) const noexcept;

        /**
         * Formatted output
         * @param stream Stream to use
         * @param path Path to print
         * @return The stream
         */
        friend std::ostream& operator<<(std::ostream& stream, ModelPath const& path);

    private:
        std::vector<RowColumnIdx> m_path;
    };
}

#endif //NOVELIST_MODELPATH_H
