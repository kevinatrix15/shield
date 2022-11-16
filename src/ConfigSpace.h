/**
 * @file ConfigSpace.h
 * @brief File containing class definitions and data structures used for
 * representing the configuration space.
 * @author Kevin Briggs <kevinabriggs@hotmail.com>
 * @version 1
 * @date 2022-11-16
 */
#pragma once

#include "Cell.h"
#include "Grid.h"

#include <algorithm>
#include <numeric>
#include <ostream>
#include <string>
#include <vector>

/**
 * @brief Enumerator for indicating the type of each cell in the domain.
 */
enum class cell_state : uint8_t { FREE, OBJECT, PADDED };

/**
 * @brief Class used for defining the configuration space, including obstacles.
 * NOTE: the domain boundaries and obstacles are padded by the robot's radius to
 * limit its work space.
 */
class ConfigurationSpace : public GridIndexer
{
  public:
  // NOTE: we assume the robot radius an integer value, which can be zero as a
  // special case
  /**
   * @brief Construct a new Configuration Space object.
   *
   * @param numX The number of cells in the x-dimension in the task space.
   * @param numY The number of cells in the y-dimension in the task space.
   * @param robotRadius The robot radius, in number of cells.
   */
  ConfigurationSpace(const size_t numX,
                     const size_t numY,
                     const size_t robotRadius)
      : GridIndexer(numX, numY),
        m_robotRadius(robotRadius),
        m_cellStates(std::make_pair(numX, numY), cell_state::FREE)
  {
    // set the cell state to 'padded' to account for robot radius
    assignBoundaryCellStates();
  }

  /**
   * @brief Construct a new Configuration Space object, taking a pre-constructed
   * map of cell states.
   *
   * @param cellStates The pre-constructed cell states.
   * @param robotRadius The robot's radius, in number of cells.
   */
  ConfigurationSpace(const DataMap<cell_state>& cellStates,
                     const size_t robotRadius)
      : GridIndexer(cellStates),
        m_robotRadius(robotRadius),
        m_cellStates(cellStates)
  {
    assignBoundaryCellStates();
  }

  /**
   * @brief Add circular obstacles to the configuration space.
   * NOTE: Padding is added around each object to account for the robot's
   * radius.
   * TODO: FUTURE WORK- create polymorphic obstacles to enable different shapes.
   *
   * @param obstacles The obstacles to add.
   */
  void addObstacles(const std::vector<Circle>& obstacles)
  {
    // add padding around obstacles
    for (const auto& obstacle : obstacles) {
      // extend obstacles with robot radius
      const Circle padded(obstacle.center(), obstacle.radius() + m_robotRadius);

      // Add padding around circle obstacles to account for robot radius
      // TODO: FUTURE WORK- this revisits the same cells as below. Add visitor
      // method which only visits the padded region to reduce cost
      GridCircle::visit(
          padded, *this, [this](const size_t xIdx, const size_t yIdx) {
            m_cellStates.at(xIdx, yIdx) = cell_state::PADDED;
          });

      // Mark the obstacles
      GridCircle::visit(
          obstacle, *this, [this](const size_t xIdx, const size_t yIdx) {
            m_cellStates.at(xIdx, yIdx) = cell_state::OBJECT;
          });
    }
  }

  /**
   * @brief Check whether a cell is accessible (i.e., within the task space and
   * unblocked).
   *
   * @param c The cell to check.
   * @return true If accessible, else false.
   */
  bool isAccessible(const Cell& c) const
  {
    return contains(c) && m_cellStates.at(c) == cell_state::FREE;
  }

  /**
   * @brief Get all accessible neighbors of a given cell, where accessibility is
   * defined by the above method. A given cell can have up to eight neighbors.
   *
   * @param c The cell to check.
   * @return std::vector<Cell> The set of accessible neighbors.
   */
  std::vector<Cell> getAccessibleNbrs(const Cell& c) const
  {
    std::vector<Cell> nbrs;
    const size_t minX = c.x() > 0 ? c.x() - 1 : c.x();
    const size_t minY = c.y() > 0 ? c.y() - 1 : c.y();
    const size_t maxX = c.x() < numX() - 1 ? c.x() + 1 : c.x();
    const size_t maxY = c.y() < numY() - 1 ? c.y() + 1 : c.y();
    for (size_t yIdx = minY; yIdx <= maxY; ++yIdx) {
      for (size_t xIdx = minX; xIdx <= maxX; ++xIdx) {
        if (m_cellStates.at(xIdx, yIdx) == cell_state::FREE) {
          nbrs.emplace_back(Cell(xIdx, yIdx));
        }
      }
    }
    return nbrs;
  }

  size_t robotRadius() const
  {
    return m_robotRadius;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const ConfigurationSpace& space)
  {
    return os << space.m_cellStates;
  };

  private:
  size_t m_robotRadius;
  DataMap<cell_state> m_cellStates;

  /**
   * @brief Assign padding to the cells within the robot's radius around the
   * outside of the task space.
   */
  void assignBoundaryCellStates()
  {
    // TODO: add visitRowsAndCols() to Grid class, providing a range
    // TODO: consider moving the below into the Grid class for specialized
    // iteration
    std::vector<size_t> allCols(numX());
    std::iota(allCols.begin(), allCols.end(), 0);

    std::vector<size_t> allRows(numY());
    std::iota(allRows.begin(), allRows.end(), 0);

    std::vector<size_t> bottomRows(m_robotRadius);
    std::iota(bottomRows.begin(), bottomRows.end(), 0);

    std::vector<size_t> topRows(m_robotRadius);
    std::iota(topRows.begin(), topRows.end(), numY() - m_robotRadius);

    std::vector<size_t> leftCols(m_robotRadius);
    std::iota(leftCols.begin(), leftCols.end(), 0);

    std::vector<size_t> rightCols(m_robotRadius);
    std::iota(rightCols.begin(), rightCols.end(), numX() - m_robotRadius);

    // bottom rows
    padRowsAndCols(bottomRows, allCols);

    // top rows
    padRowsAndCols(topRows, allCols);

    // left cols
    padRowsAndCols(allRows, leftCols);

    // right cols
    padRowsAndCols(allRows, rightCols);
  }

  void padRowsAndCols(const std::vector<size_t>& rows,
                      const std::vector<size_t>& cols)
  {
    for (const size_t yIdx : rows) {
      for (const size_t xIdx : cols) {
        m_cellStates.at(xIdx, yIdx) = cell_state::PADDED;
      }
    }
  }
};