#pragma once

#include "dida/vector2.hpp"

namespace dida
{

enum class PerturbationVector2
{
  /// The perturbation vector <tt>[-eps, -eps^2]</tt>, with Infinitesimal @c eps.
  left_down,

  /// The perturbation vector <tt>[eps, eps^2]</tt>, with Infinitesimal @c eps.
  right_up,
};

/// Returns whether the dot product of vector @c a and perturbation vector @c b is positive.
///
/// @param a The vector operand.
/// @param b The perturbation vector operand.
/// @return True iff the dot product of @c a and the perturbation vector @c b is positive.
inline bool dot_is_positive(Vector2 a, PerturbationVector2 b);

/// Returns whether the dot product of vector @c a and perturbation vector @c b is negative.
///
/// @param a The vector operand.
/// @param b The perturbation vector operand.
/// @return True iff the dot product of @c a and the perturbation vector @c b is negative.
inline bool dot_is_negative(Vector2 a, PerturbationVector2 b);

/// Returns whether the cross product of vector @c a and perturbation vector @c b is positive.
///
/// @param a The vector operand.
/// @param b The perturbation vector operand.
/// @return True iff the cross product of @c a and the perturbation vector @c b is positive.
inline bool cross_is_positive(Vector2 a, PerturbationVector2 b);

/// Returns whether the cross product of vector @c a and perturbation vector @c b is negative.
///
/// @param a The vector operand.
/// @param b The perturbation vector operand.
/// @return True iff the cross product of @c a and the perturbation vector @c b is negative.
inline bool cross_is_negative(Vector2 a, PerturbationVector2 b);

} // namespace dida::detail

#include "dida/perturbation.inl"