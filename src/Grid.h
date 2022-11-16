/**
 * @file Grid.h
 * @brief File containing class definitions related to representing and
 * accessing data on a grid.
 * @author Kevin Briggs <kevinabriggs@hotmail.com>
 * @version 1
 * @date 2022-11-16
 */
#pragma once

#include "Cell.h"

#include <cassert>
#include <functional>
#include <memory>

/**
 * @brief Class used to provide information and convenient indexing of a 2D
 * cartesian grid of cells with unit cell lengths (i.e., pixel map)
 *
 */
class GridIndexer
{
  public:
  GridIndexer(const size_t nx, const size_t ny) : m_nx(nx), m_ny(ny)
  {
    // do nothing
  }

  GridIndexer(const std::pair<size_t, size_t>& shape)
      : GridIndexer(shape.first, shape.second)
  {
    // do nothing
  }

  GridIndexer(const GridIndexer& other) : m_nx(other.m_nx), m_ny(other.m_ny)
  {
    // do nothing
  }

  /**
   * @brief Get the collapsed 1D index, given the 2D cartesian indices.
   *
   * @param xIdx The x-index.
   * @param yIdx The y-index.
   * @return size_t The 1D index.
   */
  size_t idxFrom(const size_t xIdx, const size_t yIdx) const
  {
    assert(xIdx < m_nx);
    assert(yIdx < m_ny);
    return xIdx + yIdx * m_nx;
  }

  /**
   * @brief Get the collapsed 1D index, given a cell representing the x-y
   * coordinates.
   *
   * @param c The cell.
   * @param yIdx The y-index.
   * @return size_t The 1D index.
   */
  size_t idxFrom(const Cell& c) const
  {
    return idxFrom(c.x(), c.y());
  }

  size_t size() const
  {
    return m_nx * m_ny;
  }

  size_t numX() const
  {
    return m_nx;
  }

  size_t numY() const
  {
    return m_ny;
  }

  std::pair<size_t, size_t> shape() const
  {
    return std::make_pair(numX(), numY());
  }

  // NOTE: we are using Cells here to store the x and y indices for convenience.
  bool contains(const Cell& c) const
  {
    return c.x() < numX() && c.y() < numY();
  }

  private:
  size_t m_nx;
  size_t m_ny;
};

/**
 * @brief Class used for storing data on a 2D grid, with convenience methods for
 * natural data access, while storing data in a 1D vector for improved cache
 * performance.
 *
 * @tparam T The data type.
 */
template <typename T>
class DataMap : public GridIndexer
{
  public:
  DataMap(const std::pair<size_t, size_t>& shape)
      : GridIndexer(shape), m_data(size())
  {
    // do nothing
  }

  DataMap(const std::pair<size_t, size_t>& shape, const std::vector<T>& data)
      : GridIndexer(shape), m_data(data)
  {
    // do nothing
  }

  DataMap(const std::pair<size_t, size_t>& shape, const T& initVal)
      : GridIndexer(shape), m_data(size(), initVal)
  {
    // do nothing
  }

  T at(const size_t xIdx, const size_t yIdx) const
  {
    return m_data[GridIndexer::idxFrom(xIdx, yIdx)];
  }

  T at(const Cell& c) const
  {
    return m_data[GridIndexer::idxFrom(c)];
  }

  typename std::vector<T>::reference at(const size_t xIdx, const size_t yIdx)
  {
    return m_data[GridIndexer::idxFrom(xIdx, yIdx)];
  }

  typename std::vector<T>::reference at(const Cell& c)
  {
    return m_data[GridIndexer::idxFrom(c)];
  }

  friend std::ostream& operator<<(std::ostream& os, const DataMap& dataMap)
  {
    for (size_t yIdx = 0; yIdx < dataMap.numY(); ++yIdx) {
      for (size_t xIdx = 0; xIdx < dataMap.numX(); ++xIdx) {
        os << static_cast<int>(dataMap.at(xIdx, yIdx))
           << (xIdx < dataMap.numX() - 1 ? " " : "");
      }
      os << std::endl;
    }
    return os;
  };

  private:
  std::vector<T> m_data;
};

/**
 * @brief Class for representing a circle.
 */
class Circle
{
  public:
  /**
   * @brief Construct a new Circle object
   *
   * @param center The center of the circle, as a cell.
   * @param radius The radius of the circle, in units of cells.
   */
  Circle(const Cell& center, const size_t radius)
      : m_center(center), m_radius(radius)
  {
  }

  Cell center() const
  {
    return m_center;
  }

  size_t radius() const
  {
    return m_radius;
  }

  private:
  Cell m_center;
  size_t m_radius;
};

using Callback = std::function<void(const size_t xIdx, const size_t yIdx)>;
/**
 * @brief Class for representing a circle on a 2D grid object.
 */
class GridCircle
{
  public:
  /**
   * @brief Visit all cells within the provided circle, performing a callback
   * function at each cell.
   *
   * @param circle The circle.
   * @param grid The grid object.
   * @param callback The callback to be performed.
   */
  static void visit(const Circle& circle,
                    const GridIndexer& grid,
                    Callback callback)
  {
    const size_t r = circle.radius();
    const Cell c = circle.center();

    const size_t xLimit = grid.numX() - 1;
    const size_t yLimit = grid.numY() - 1;

    auto insideLeft{[&](const size_t xIdx) { return c.x() >= xIdx; }};
    auto insideRight{[&](const size_t xIdx) { return xIdx + c.x() <= xLimit; }};
    auto insideBottom{[&](const size_t yIdx) { return c.y() >= yIdx; }};
    auto insideTop{[&](const size_t yIdx) { return yIdx + c.y() <= yLimit; }};

    // TODO: consider replacing this with a simple brute force loop over
    // bounding box, checking each point is within the circle. This will avoid
    // all of the excessive checking for each point to be within the domain
    for (size_t yIdx = 0; yIdx <= r; ++yIdx) {
      const size_t xMaxSq = r * r - yIdx * yIdx;
      size_t xIdx = 0;
      while (xIdx * xIdx < xMaxSq) {
        // TODO: only visit if within domain
        // Quadrant I
        if (insideRight(xIdx) && insideTop(yIdx)) {
          callback(c.x() + xIdx, c.y() + yIdx);
        }
        // Quadrant II
        if (insideLeft(xIdx) && insideTop(yIdx)) {
          callback(c.x() - xIdx, c.y() + yIdx);
        }
        // Quadrant III
        if (insideLeft(xIdx) && insideBottom(yIdx)) {
          callback(c.x() - xIdx, c.y() - yIdx);
        }
        // Quadrant IV
        if (insideRight(xIdx) && insideBottom(yIdx)) {
          callback(c.x() + xIdx, c.y() - yIdx);
        }
        ++xIdx;
      }
    }
  }
};