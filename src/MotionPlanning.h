/**
 * @file MotionPlanning.h
 * @brief File containing various implementations of motion planning algorithms.
 * @author Kevin Briggs <kevinabriggs@hotmail.com>
 * @version 1
 * @date 2022-11-16
 */
#pragma once

#include "Cell.h"
#include "ConfigSpace.h"

#include <iostream>
#include <limits>
#include <queue>

constexpr double UNSET_VAL = std::numeric_limits<double>::max();
constexpr size_t _UNSET_IDX = std::numeric_limits<size_t>::max();
const Cell UNSET_CELL(_UNSET_IDX, _UNSET_IDX);

// stores {fCost, Cell}
using CostCell = std::pair<double, Cell>;

/**
 * @brief Structure containing Node data for use with path finding algorithms.
 */
struct Node {
  public:
  Node()
      : pos(UNSET_CELL),
        parentPos(UNSET_CELL),
        gCost(UNSET_VAL),
        fCost(UNSET_VAL)
  {
    // do nothing
  }

  Node(const Cell& p, const Cell& parentPos) : pos(p), parentPos(parentPos)
  {
    // do nothing
  }

  void updateCosts(const Cell& goal, const double parentGCost)
  {
    // TODO: FUTURE WORK- make configurable function to determine which
    // heuristic to use
    const double hCost = pos.distance(goal);
    gCost = parentGCost + 1.0;
    fCost = gCost + hCost;
  }

  Cell pos;
  Cell parentPos;

  // gCost, fCost
  double gCost;
  double fCost;

  private:
};

/**
 * @brief Class used to perform the A* path-finding algorithm.
 */
class AStar
{
  public:
  // TODO: FUTURE WORK- provide a heuristic function to enable experimenting
  // with different heuristics.
  explicit AStar(const ConfigurationSpace& cSpace) : m_cSpace(cSpace)
  {
    // do nothing
  }

  /**
   * @brief Perform path-finding using the A* algorithm. The below
   * implementation follows the descriptions provided at the following links:
   *   https://en.wikipedia.org/wiki/A*_search_algorithm
   *   https://www.simplilearn.com/tutorials/artificial-intelligence-tutorial/a-star-algorithm#algorithm
   *   https://www.geeksforgeeks.org/a-search-algorithm/
   *
   * @param start The start location
   * @param goal The goal location
   * @return std::vector<Cell> The cell locations making up the path, ordered
   * from start to goal, if found. Otherwise, an empty vector is returned.
   */
  std::vector<Cell> searchPath(const Cell& start, const Cell& goal) const
  {
    // Check for blocked / unreachable start and goal positions, or start is at
    // the goal. Chose not to throw an exception to allow program to continue
    // with new user-provided inputs
    if (!isValidStartGoal(start, goal)) {
      return std::vector<Cell>();
    }

    // Use a priority queue as a min-heap with smallest f-cost at the top for
    // storing the unexplored (open) nodes
    auto comp = [&](const CostCell& c1, const CostCell& c2) {
      return c1.first > c2.first;
    };
    std::priority_queue<CostCell, std::vector<CostCell>, decltype(comp)>
        unexploredNodes(comp);

    // Container storing the current state of each node in the map
    const std::pair<size_t, size_t> mapShape = m_cSpace.shape();
    DataMap<Node> nodeMap(mapShape);

    // Map tracking which nodes have been explored in the search (closed)
    // TODO: consider replacing this with unordered_set
    DataMap<bool> exploredNodes(mapShape, false);

    // Put starting node on the open list (with fCost = 0 and gCost = 0)
    Node startNode(start, start);
    startNode.fCost = 0.0;
    startNode.gCost = 0.0;
    nodeMap.at(start) = startNode;

    CostCell startCP{startNode.fCost, start};
    unexploredNodes.emplace(startCP);

    while (!unexploredNodes.empty()) {
      // Next search node 'q' is the node with lowest fCost from the heap
      CostCell q = unexploredNodes.top();
      const Cell qPos = q.second;
      // Remove q from the top of the heap and add it to the explored nodes
      unexploredNodes.pop();
      exploredNodes.at(qPos) = true;

      // Get all of the current node's accessible neighbors.
      // There are 8 max possible neighbors, but may be less if near
      // the border or within an obstacle
      const std::vector<Cell> nbrCells = m_cSpace.getAccessibleNbrs(qPos);
      for (const auto& nbrCell : nbrCells) {
        // Check if this neighbor is the goal, updates its state, and
        // return a path
        if (nbrCell == goal) {
          std::cout << "Goal found!!!" << std::endl;
          Node nbr(nbrCell, qPos);
          nodeMap.at(nbr.pos) = nbr;
          return generatePath(nodeMap, goal);
        }

        // Explore this neighbor if we haven't already
        if (!exploredNodes.at(nbrCell)) {
          Node nbr(nbrCell, qPos);
          const double parentGCost = nodeMap.at(qPos).gCost;
          nbr.updateCosts(goal, parentGCost);

          // Add this neighbor to the unexplored nodes
          // if not on the open list, add to open list, and set current cell as
          // the parent
          //         OR
          // if on the open list, check if has a smaller f
          if (nodeMap.at(nbr.pos).fCost == UNSET_VAL ||
              nodeMap.at(nbr.pos).fCost > nbr.fCost) {
            unexploredNodes.emplace(std::make_pair(nbr.fCost, nbr.pos));
            nodeMap.at(nbr.pos) = nbr;
          }
        }
      }
    }
    std::cout << "Goal not found... :(" << std::endl;
    return std::vector<Cell>();
  }

  private:
  ConfigurationSpace m_cSpace;

  /**
   * @brief Check the start and goal positions to ensure they are valid. Incalid
   * cases include:
   *  - Start is outside of the domain
   *  - Goal is outside of the domain
   *  - Start is not accessible
   *  - Goal is not accessible
   *  - Start is already at the goal
   *
   * @param start The start position
   * @param goal The goal position
   * @return false if one of the above conditions, else true
   */
  bool isValidStartGoal(const Cell& start, const Cell& goal) const
  {
    if (!m_cSpace.contains(start)) {
      std::cout << "Start point " << start << " is not in the grid"
                << std::endl;
      return false;
    }

    if (!m_cSpace.contains(goal)) {
      std::cout << "Goal point " << start << " is not in the grid" << std::endl;
      return false;
    }

    if (!m_cSpace.isAccessible(start)) {
      std::cout << "Start point " << start << " is not accessible" << std::endl;
      return false;
    }

    if (!m_cSpace.isAccessible(goal)) {
      std::cout << "Goal point " << goal << " is not accessible" << std::endl;
      return false;
    }

    if (start == goal) {
      std::cout << "Start position " << start << " is already at goal"
                << std::endl;
      return false;
    }
    return true;
  }

  /**
   * @brief Generate the path followed from start to goal
   *
   * @param nodeMap The node map containing node data at each grid point
   * @param goal The goal location
   * @return std::vector<Cell> All points followed from start to goal
   */
  static std::vector<Cell> generatePath(const DataMap<Node>& nodeMap,
                                        const Cell& goal)
  {
    std::vector<Cell> path;

    // Generate the path, working backwards from the goal
    // node, and terminating when we reach the start location
    path.emplace_back(goal);
    Cell next = nodeMap.at(goal).parentPos;
    do {
      path.emplace_back(next);
      next = nodeMap.at(next).parentPos;
    } while (next != nodeMap.at(next).parentPos);

    // Reverse the order to go from start to goal
    std::reverse(path.begin(), path.end());
    return path;
  }
};
