#pragma once

#include <vector>

namespace ko {

/**
 * Represents a matrix, i.e., a two dimensional array.
 *
 * @tparam Element The element type of the matrix
 */
template <typename Element>
class Matrix {
    /// The element type
    using ElementType = Element;
    /// Reference to the element type
    using ReferenceType = typename std::vector<ElementType>::reference;
    /// Const reference to the element type
    using ConstReferenceType = typename std::vector<ElementType>::const_reference;

public:
    /**
     * Constructs a matrix with a given number of rows and columns.
     *
     * @param rows Number of rows
     * @param cols Number of columns
     */
    Matrix(std::size_t rows, std::size_t cols)
      : m_rows(rows)
      , m_cols(cols)
      , m_data(rows * cols, ElementType()) {}

    /**
     * Constructs a quadratic matrix with a given dimension.
     *
     * @param n The given dimension
     */
    explicit Matrix(std::size_t n)
      : Matrix(n, n) {}

    /**
     * Constructs an empty matrix.
     */
    Matrix()
      : m_rows(0)
      , m_cols(0)
      , m_data{ 0 } {}

    /**
     * Returns a reference to a given element.
     *
     * @param i The element's row index
     * @param j The element's column index
     * @return Reference to the element
     */
    ReferenceType operator()(std::size_t i, std::size_t j) { return this->m_data[i * this->m_cols + j]; }

    /**
     * Returns a const reference to a given element.
     *
     * @param i The element's row index
     * @param j The element's column index
     * @return Const reference to the element
     */
    ConstReferenceType operator()(std::size_t i, std::size_t j) const { return this->m_data[i * this->m_cols + j]; }

    /**
     * Returns the number of rows.
     *
     * @return The number of rows.
     */
    [[nodiscard]] std::size_t rows() const { return this->m_rows; }

    /**
     * Returns the number of columns.
     *
     * @return The number of columns.
     */
    [[nodiscard]] std::size_t cols() const { return this->m_cols; }

private:
    /// The number of rows
    std::size_t m_rows{};
    /// The number of columns
    std::size_t m_cols{};
    /// The vector containing the matrix element data
    std::vector<Element> m_data;
};

} // namespace ko