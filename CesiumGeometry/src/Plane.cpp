#include "CesiumGeometry/Plane.h"

#include <CesiumUtility/Assert.h>
#include <CesiumUtility/Math.h>

#include <glm/geometric.hpp>

using namespace CesiumUtility;

namespace CesiumGeometry {

// NOLINTBEGIN(cert-err58-cpp)
const Plane Plane::ORIGIN_XY_PLANE{glm::dvec3(0.0, 0.0, 1.0), 0.0};
const Plane Plane::ORIGIN_YZ_PLANE{glm::dvec3(1.0, 0.0, 0.0), 0.0};
const Plane Plane::ORIGIN_ZX_PLANE{glm::dvec3(0.0, 1.0, 0.0), 0.0};
// NOLINTEND(cert-err58-cpp)

Plane::Plane() noexcept : Plane(glm::dvec3(0.0, 0.0, 1.0), 0.0) {}

Plane::Plane(const glm::dvec3& normal, double distance)
    : _normal(normal), _distance(distance) {
  CESIUM_ASSERT(
      Math::equalsEpsilon(glm::length(normal), 1.0, Math::Epsilon6) &&
      "normal must be normalized.");
}

Plane::Plane(const glm::dvec3& point, const glm::dvec3& normal)
    : Plane(normal, -glm::dot(normal, point)) {}

double Plane::getPointDistance(const glm::dvec3& point) const noexcept {
  return glm::dot(this->_normal, point) + this->_distance;
}

glm::dvec3
Plane::projectPointOntoPlane(const glm::dvec3& point) const noexcept {
  // projectedPoint = point - (normal.point + scale) * normal
  const double pointDistance = this->getPointDistance(point);
  const glm::dvec3 scaledNormal = this->_normal * pointDistance;
  return point - scaledNormal;
}

} // namespace CesiumGeometry
