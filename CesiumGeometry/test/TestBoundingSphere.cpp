#include "CesiumGeometry/BoundingSphere.h"
#include "CesiumGeometry/Plane.h"
#include "CesiumUtility/Math.h"

#include <CesiumGeometry/CullingResult.h>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <glm/ext/matrix_double3x3.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>

using namespace CesiumGeometry;
using namespace CesiumUtility;

TEST_CASE("BoundingSphere::intersectPlane") {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
  struct TestCase {
    BoundingSphere sphere;
    Plane plane;
    CullingResult expectedResult;
  };

  auto testCase = GENERATE(
      // sphere on the positive side of a plane
      TestCase{
          BoundingSphere(glm::dvec3(0.0), 0.5),
          Plane(glm::dvec3(-1.0, 0.0, 0.0), 1.0),
          CullingResult::Inside},
      // sphere on the negative side of a plane
      TestCase{
          BoundingSphere(glm::dvec3(0.0), 0.5),
          Plane(glm::dvec3(1.0, 0.0, 0.0), -1.0),
          CullingResult::Outside},
      // sphere intersection a plane
      TestCase{
          BoundingSphere(glm::dvec3(1.0, 0.0, 0.0), 0.5),
          Plane(glm::dvec3(1.0, 0.0, 0.0), -1.0),
          CullingResult::Intersecting});

  CHECK(
      testCase.sphere.intersectPlane(testCase.plane) ==
      testCase.expectedResult);
}

TEST_CASE(
    "BoundingSphere::computeDistanceSquaredToPosition test outside sphere") {
  BoundingSphere bs(glm::dvec3(0.0), 1.0);
  glm::dvec3 position(-2.0, 1.0, 0.0);
  double expected = 1.52786405;
  CHECK(CesiumUtility::Math::equalsEpsilon(
      bs.computeDistanceSquaredToPosition(position),
      expected,
      CesiumUtility::Math::Epsilon6));
}

TEST_CASE(
    "BoundingSphere::computeDistanceSquaredToPosition test inside sphere") {
  BoundingSphere bs(glm::dvec3(0.0), 1.0);
  glm::dvec3 position(-.5, 0.5, 0.0);
  CHECK(bs.computeDistanceSquaredToPosition(position) == 0);
}

TEST_CASE("BoundingSphere::contains") {
  glm::dvec3 center(1.0, 2.0, 3.0);
  double radius = 45.0;
  BoundingSphere sphere(center, radius);
  double epsilon = CesiumUtility::Math::Epsilon14;

  CHECK(sphere.contains(center));
  CHECK(sphere.contains(center + glm::dvec3(radius, 0.0, 0.0)));
  CHECK(!sphere.contains(center + glm::dvec3(radius + epsilon, 0.0, 0.0)));
}

TEST_CASE("BoundingSphere::transform") {
  BoundingSphere sphere(glm::dvec3(1.0, 2.0, 3.0), 45.0);

  SECTION("translating moves the center only") {
    glm::dmat4 transformation = glm::translate(
        glm::identity<glm::dmat4>(),
        glm::dvec3(10.0, 20.0, 30.0));
    BoundingSphere transformed = sphere.transform(transformation);
    CHECK(transformed.getRadius() == Catch::Approx(sphere.getRadius()));
    CHECK(
        transformed.getCenter().x ==
        Catch::Approx(sphere.getCenter().x + 10.0));
    CHECK(
        transformed.getCenter().y ==
        Catch::Approx(sphere.getCenter().y + 20.0));
    CHECK(
        transformed.getCenter().z ==
        Catch::Approx(sphere.getCenter().z + 30.0));
  }

  SECTION("rotating moves the center only") {
    double fortyFiveDegrees = Math::OnePi / 4.0;
    glm::dmat4 transformation = glm::eulerAngleY(fortyFiveDegrees);
    BoundingSphere transformed = sphere.transform(transformation);
    CHECK(transformed.getRadius() == Catch::Approx(sphere.getRadius()));

    glm::dvec3 rotatedCenter = glm::dmat3(transformation) * sphere.getCenter();
    CHECK(transformed.getCenter().x == Catch::Approx(rotatedCenter.x));
    CHECK(transformed.getCenter().y == Catch::Approx(rotatedCenter.y));
    CHECK(transformed.getCenter().z == Catch::Approx(rotatedCenter.z));
  }

  SECTION(
      "scaling moves the center, and scales the radius by the max component") {
    glm::dmat4 transformation =
        glm::scale(glm::identity<glm::dmat4>(), glm::dvec3(2.0, 3.0, 4.0));
    BoundingSphere transformed = sphere.transform(transformation);

    glm::dvec3 scaledCenter = glm::dmat3(transformation) * sphere.getCenter();
    CHECK(transformed.getCenter().x == Catch::Approx(scaledCenter.x));
    CHECK(transformed.getCenter().y == Catch::Approx(scaledCenter.y));
    CHECK(transformed.getCenter().z == Catch::Approx(scaledCenter.z));

    CHECK(transformed.getRadius() == Catch::Approx(45.0 * 4.0));
  }
}

TEST_CASE("BoundingSphere::computeDistanceSquaredToPosition example") {
  auto anyOldSphereArray = []() {
    return std::vector<BoundingSphere>{
        {glm::dvec3(1.0, 0.0, 0.0), 1.0},
        {glm::dvec3(2.0, 0.0, 0.0), 1.0}};
  };
  auto anyOldCameraPosition = []() { return glm::dvec3(0.0, 0.0, 0.0); };

  //! [distanceSquaredTo]
  // Sort bounding spheres from back to front
  glm::dvec3 cameraPosition = anyOldCameraPosition();
  std::vector<BoundingSphere> spheres = anyOldSphereArray();
  std::sort(
      spheres.begin(),
      spheres.end(),
      [&cameraPosition](auto& a, auto& b) {
        return a.computeDistanceSquaredToPosition(cameraPosition) >
               b.computeDistanceSquaredToPosition(cameraPosition);
      });
  //! [distanceSquaredTo]

  CHECK(spheres[0].getCenter().x == 2.0);
  CHECK(spheres[1].getCenter().x == 1.0);
}
