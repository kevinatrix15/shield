/**
 * @file Cell.h
 * @brief File containing the definition of the Cell class.
 * @author Kevin Briggs <kevinabriggs@hotmail.com>
 * @version 1
 * @date 2022-11-16
 */
#pragma once

#include <cassert>
#include <cmath>
#include <ostream>

/**
 * @brief Class for representing a unit cell, with x-y index coordinates. This
 * enables using a Cell object for indexing into a 2D map.
 */
class Cell
{
  public:
  Cell() : m_x(0U), m_y(0U)
  {
    // do nothing
  }

  Cell(const size_t x, const size_t y) : m_x(x), m_y(y)
  {
    // do nothing
  }

  size_t x() const
  {
    return m_x;
  }

  size_t y() const
  {
    return m_y;
  }

  size_t& x()
  {
    return m_x;
  }

  size_t& y()
  {
    return m_y;
  }

  /**
   * @brief Calculate the Euclidean distance between two cells.
   *
   * @param other The other cell.
   * @return double The distance. NOTE: this is returned as a double to avoid
   * truncation with non-orthogonal distances.
   */
  double distance(const Cell& other) const
  {
    const double dx = static_cast<double>(m_x) - static_cast<double>(other.m_x);
    const double dy = static_cast<double>(m_y) - static_cast<double>(other.m_y);
    return sqrt(dx * dx + dy * dy);
  }

  bool operator==(const Cell& other) const
  {
    return m_x == other.m_x && m_y == other.m_y;
  }

  bool operator!=(const Cell& other) const
  {
    return !(*this == other);
  }

  friend std::ostream& operator<<(std::ostream& os, const Cell& p)
  {
    os << "(" << p.x() << ", " << p.y() << ")";
    return os;
  };

  private:
  size_t m_x;
  size_t m_y;
};