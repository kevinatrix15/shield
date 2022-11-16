/**
 * @file CellTests.cpp
 * @brief Unit tests for the Cell class.
 * @author Kevin Briggs <kevinabriggs@hotmail.com>
 * @version 1
 * @date 2022-11-16
 */
#include "Cell.h"

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one file
#include "catch2.h"

#include <cmath>
#include <string>

namespace
{
void testDist(const std::string& description,
              const Cell& c1,
              const Cell& c2,
              const double expected)
{
  SECTION(description)
  {
    try {
      // arrange & act
      const double dist = c1.distance(c2);

      // assert
      REQUIRE(expected == dist);
    } catch (const std::exception& e) {
      FAIL(e.what());
    }
  }
}
} // namespace

TEST_CASE("Cell construction and getters are valid", "[ctor]")
{
  // arrange
  Cell c(4, 2);

  // act
  const size_t x = c.x();
  const size_t y = c.y();

  // assert
  REQUIRE(4 == x);
  REQUIRE(2 == y);
}

TEST_CASE("Cell setters are valid", "setters")
{
  // arrange
  Cell c;

  // act
  c.x() = 4;
  c.y() = 2;

  // assert
  REQUIRE(4 == c.x());
  REQUIRE(2 == c.y());
}

TEST_CASE("Distance is valid for diagonal test", "setters")
{
  testDist("coincident1", {0, 0}, {0, 0}, 0.0);
  testDist("coincident2", {4, 2}, {4, 2}, 0.0);

  testDist("unit vectors1", {0, 1}, {0, 0}, 1.0);
  testDist("unit vectors2", {0, 0}, {1, 0}, 1.0);

  testDist("unit diagonal", {1, 1}, {0, 0}, sqrt(2.0));
  // (size_t)(5 * sqrt(2)) = (size_t)(7.0711) = 7
  testDist("diagonal- 5*sqrt(2)", {0, 0}, {5, 5}, 5.0 * sqrt(2.0));

  // 3 4 5 triangles
  testDist("3 4 5 triangle", {0, 3}, {4, 0}, 5.0);
  testDist("3 4 5 triangle- flipped", {3, 0}, {0, 4}, 5.0);
}