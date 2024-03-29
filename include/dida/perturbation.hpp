#pragma once

#include "dida/vector2.hpp"

namespace dida
{

/// A symbolic infinitesimal offset which can be applied to parts of the input of predicates to symbolically turn input
/// in special position into input in general position.
///
/// Formally, the output of a function which takes a @c PerturbationVector2 offset to be applied to some of its input
/// will be equal to the limit of the output as a sequence of positive @c eps approach 0.
enum class PerturbationVector2
{
  /// The perturbation vector <tt>[-eps, -eps^2]</tt>, with Infinitesimal @c eps.
  left_down,

  /// The perturbation vector <tt>[eps, eps^2]</tt>, with Infinitesimal @c eps.
  right_up,
};

/// Returns the perturbation vector in the direction opposite to @c a.
///
/// @param a The original perturbation vector.
/// @return The negated perturbation vector.
constexpr PerturbationVector2 negate_perturbation_vector(PerturbationVector2 a);

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

} // namespace dida

#include "dida/perturbation.inl"