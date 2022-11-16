/**
 * @file Main.cpp
 * @brief The main file for the save-bb8 project, used to provide motion
 * planning through a configuration space with obstacles.
 * @author Kevin Briggs <kevinabriggs@hotmail.com>
 * @version 1
 * @date 2022-11-16
 */
#include "ConfigSpace.h"
#include "FileIO.h"
#include "MotionPlanning.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <vector>

constexpr int EXPECTED_ARGS = 5;

int main(int argc, char** argv)
{
  // TODO: FUTURE WORK- add more complete command line argument handling, with
  // more robust checking and a help option.

  // args:
  // 0- this command
  // 1- M (num rows => ny)
  // 2- N (num cols => nx)
  // 3- robot radius (in cells)
  // 4- Pre-configured case
  // Usage:
  // ./<this-executable> <M> <N> <robot-radius> <pre-configured-case>
  // ./save-bb8 100 250 6 3
  if (argc != EXPECTED_ARGS) {
    throw std::runtime_error(
        "Invalid number of args provided: " + std::to_string(argc) +
        ", expected " + std::to_string(EXPECTED_ARGS));
  }
  const size_t ny = static_cast<size_t>(std::stoi(argv[1]));
  const size_t nx = static_cast<size_t>(std::stoi(argv[2]));
  const size_t robotRadius = static_cast<size_t>(std::stoi(argv[3]));

  // Pre-configured obstacle cases:
  // 1- no obstacles
  // 2- impossible path- single circle in center of domain = max(M, N)
  // 3- simple path, following diagonal- circles at opposite corners from start
  // and goal 4- more complex path 5- Maze- staggered barriers
  enum class obstacle_config { NONE = 1, IMPOSSIBLE, SIMPLE, COMPLEX, MAZE };
  const obstacle_config obstacleCase =
      static_cast<obstacle_config>(std::stoi(argv[4]));

  ConfigurationSpace cSpace(nx, ny, robotRadius);

  // TODO: FUTURE WORK- make interactive assignment of obstacles
  std::vector<Circle> obstacles;
  switch (obstacleCase) {
    case obstacle_config::NONE: {
      // nothing to do here
      break;
    }
    case obstacle_config::IMPOSSIBLE: {
      // single circle in center of domain with radius spanning the narrow
      // dimension
      const size_t minRadius = std::min(nx, ny) / 2;
      const Cell midPt = Cell(nx / 2, ny / 2);
      obstacles.emplace_back(Circle(midPt, minRadius));
      break;
    }
    case obstacle_config::SIMPLE: {
      // two circles at opposite corners
      const size_t radius = static_cast<int>(std::min(nx, ny) / 2) -
                            static_cast<int>(robotRadius);
      obstacles.emplace_back(Circle({0, ny - 1}, radius));
      obstacles.emplace_back(Circle({nx - 1, 0}, radius));
      break;
    }
    case obstacle_config::COMPLEX: {
      // two circles at opposite corners
      const size_t radius = static_cast<int>(std::min(nx, ny) / 8) -
                            static_cast<int>(robotRadius);
      obstacles.emplace_back(Circle({0, ny / 4}, radius));
      obstacles.emplace_back(Circle({0, ny / 2}, radius));
      obstacles.emplace_back(Circle({0, 3 * ny / 4}, radius));

      obstacles.emplace_back(Circle({nx / 4, 0}, radius));
      obstacles.emplace_back(Circle({nx / 4, ny / 3}, radius));
      obstacles.emplace_back(Circle({nx / 4, 2 * ny / 3}, radius));
      obstacles.emplace_back(Circle({nx / 4, ny - 1}, radius));

      obstacles.emplace_back(Circle({nx / 2, ny / 4}, radius));
      obstacles.emplace_back(Circle({nx / 2, ny / 2}, radius));
      obstacles.emplace_back(Circle({nx / 2, 3 * ny / 4}, radius));

      obstacles.emplace_back(Circle({3 * nx / 4, 0}, radius));
      obstacles.emplace_back(Circle({3 * nx / 4, ny / 3}, radius));
      obstacles.emplace_back(Circle({3 * nx / 4, 2 * ny / 3}, radius));
      obstacles.emplace_back(Circle({3 * nx / 4, ny - 1}, radius));

      obstacles.emplace_back(Circle({nx - 1, ny / 4}, radius));
      obstacles.emplace_back(Circle({nx - 1, ny / 2}, radius));
      obstacles.emplace_back(Circle({nx - 1, 3 * ny / 4}, radius));

      break;
    }
    case obstacle_config::MAZE: {
      // two circles at opposite corners
      // TODO: fix the below- it runs into nx and is out of bounds
      const size_t radius = static_cast<int>(std::min(nx, ny) / 10) -
                            static_cast<int>(robotRadius);
      obstacles.emplace_back(Circle({nx / 5, 0}, radius));
      obstacles.emplace_back(Circle({nx / 5, ny / 6}, radius));
      obstacles.emplace_back(Circle({nx / 5, ny / 3}, radius));
      obstacles.emplace_back(Circle({nx / 5, ny / 2}, radius));
      obstacles.emplace_back(Circle({nx / 5, 2 * ny / 3}, radius));
      obstacles.emplace_back(Circle({nx / 5, 5 * ny / 6}, radius));

      obstacles.emplace_back(Circle({2 * nx / 5, ny / 6}, radius));
      obstacles.emplace_back(Circle({2 * nx / 5, ny / 3}, radius));
      obstacles.emplace_back(Circle({2 * nx / 5, ny / 2}, radius));
      obstacles.emplace_back(Circle({2 * nx / 5, 2 * ny / 3}, radius));
      obstacles.emplace_back(Circle({2 * nx / 5, 5 * ny / 6}, radius));
      obstacles.emplace_back(Circle({2 * nx / 5, ny - 1}, radius));

      obstacles.emplace_back(Circle({3 * nx / 5, 0}, radius));
      obstacles.emplace_back(Circle({3 * nx / 5, ny / 6}, radius));
      obstacles.emplace_back(Circle({3 * nx / 5, ny / 3}, radius));
      obstacles.emplace_back(Circle({3 * nx / 5, ny / 2}, radius));
      obstacles.emplace_back(Circle({3 * nx / 5, 2 * ny / 3}, radius));
      obstacles.emplace_back(Circle({3 * nx / 5, 5 * ny / 6}, radius));

      obstacles.emplace_back(Circle({4 * nx / 5, ny / 6}, radius));
      obstacles.emplace_back(Circle({4 * nx / 5, ny / 3}, radius));
      obstacles.emplace_back(Circle({4 * nx / 5, ny / 2}, radius));
      obstacles.emplace_back(Circle({4 * nx / 5, 2 * ny / 3}, radius));
      obstacles.emplace_back(Circle({4 * nx / 5, 5 * ny / 6}, radius));
      obstacles.emplace_back(Circle({4 * nx / 5, ny - 1}, radius));
      break;
    }
    default:
      throw std::runtime_error("Invalid obstacleCase argument: " +
                               std::to_string(static_cast<int>(obstacleCase)));
  }

  // Add the obstacles to the configuration space
  cSpace.addObstacles(obstacles);

  // Write the configuration space to a file
  const std::filesystem::path cSpaceFile("./output/config-space.txt");
  ConfigSpaceIO::write(cSpace, cSpaceFile);

  // Read the configuration space back in and assign to a new object
  ConfigurationSpace cSpace2 = ConfigSpaceIO::read(cSpaceFile);

  // Search for a solution, with the starting position at the bottom corner, and
  // goal at opposite corner
  AStar search(cSpace2);
  const std::vector<Cell> path = search.searchPath(
      {robotRadius + 1, robotRadius + 1},
      {cSpace.numX() - robotRadius - 1, cSpace.numY() - robotRadius - 1});

  // Write the solved path to a file
  const std::filesystem::path pathFile("./output/solution-path.txt");
  SolutionPathIO::write(path, pathFile);

  return 0;
}