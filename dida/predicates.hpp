#pragma once

#include "dida/perturbation.hpp"
#include "dida/polygon2.hpp"

namespace dida
{

/// Returns whether @c point is within @c polygon. Corner cases are resolved by perturbing point by the infinitisemal @c
/// point_perturbation_vector.
///
/// @tparam point_perturbation_vector The perturbation vector.
/// @param polygon The polygon.
/// @param point The point.
template <PerturbationVector2 point_perturbation_vector>
bool is_within_with_perturbation(PolygonView2 polygon, Point2 point);

} // namespace dida