#pragma once

#include <optional>

#include "dida/perturbation.hpp"
#include "dida/point2.hpp"

namespace dida
{

/// A 2D segment.
class Segment2
{
public:
  /// Constructs an uninitialized @c Segment2.
  Segment2() = default;

  /// Constructs a segment with the given start and end points.
  ///
  /// @pre The two end points must be distinct. It's verified using a @c DIDA_ASSERT that this is the case.
  /// @param start The start point.
  /// @param end The end point.
  inline Segment2(Point2 start, Point2 end);

  /// Constructs a segment with the given start and end points.
  ///
  /// @pre The two end points must be distinct. It's verified using a @c DIDA_DEBUG_ASSERT that this is the case.
  /// @param start The start point.
  /// @param end The end point.
  static inline Segment2 unsafe_from_endpoints(Point2 start, Point2 end);

  /// Returns the start point of this @c Segment2.
  ///
  /// @return The start point.
  inline Point2 start() const;

  /// Returns the end point of this @c Segment2.
  ///
  /// @return the end point.
  inline Point2 end() const;

private:
  Point2 start_;
  Point2 end_;
};

/// The crossing point of 2 segments, expressed using the interpolation parameters of the crossing point on the 2
/// segments:
///
///  crossing_point = a_start + s_num / denom * a_dir = b_start + t_num / denom * b_dir
///
/// s_num, t_num and denom always satisfy
///
///  - denom > 0
///  - 0 <= s_num / denom <= 1
///  - 0 <= t_num / denom <= 1
///
class SegmentsCrossingPointParams
{
public:
  /// Constructs a @c SegmentsCrossingPointParams with the given values.
  ///
  /// @pre The values must satisfy the condition described in the class documentation.
  /// @param s_num The numerator of the parameter corresponding to the first segment.
  /// @param s_num The numerator of the parameter corresponding to the second segment.
  /// @param denom The denominator of both parameters.
  inline SegmentsCrossingPointParams(ScalarDeg2 s_num, ScalarDeg2 t_num, ScalarDeg2 denom);

  /// Returns the numerator of the @c s parameter.
  ///
  /// @return The numerator of the @c s parameter.
  inline ScalarDeg2 s_num() const;

  /// Returns the numerator of the @c t parameter.
  ///
  /// @return The numerator of the @c t parameter.
  inline ScalarDeg2 t_num() const;

  /// Returns the denominator of both parameters.
  ///
  /// @return The denominator of both parameters.
  inline ScalarDeg2 denom() const;

  /// Compares two @c SegmentsCrossingPointParams instances for equality.
  ///
  /// Two @c SegmentsCrossingPointParams are considered equal iff both have to the same values for <tt>s = s_num /
  /// denom</tt> and <tt>t = t_num / denom</tt>, even if the values of @c s_num, @c t_num and @c denom are different.
  ///
  /// @param b The second operand.
  /// @return True iff the two @c SegmentsCrossingPointParams are equal.
  inline bool operator==(const SegmentsCrossingPointParams& b) const;

  /// Compares two @c SegmentsCrossingPointParams instances for inequality.
  ///
  /// @param b The second operand.
  /// @return True iff the two @c SegmentsCrossingPointParams are distinct.
  inline bool operator!=(const SegmentsCrossingPointParams& b) const;

private:
  ScalarDeg2 s_num_;
  ScalarDeg2 t_num_;
  ScalarDeg2 denom_;
};

/// Returns the crossing point of segments @c a and @c b. Special cases are resolved by shifting segment @c b by the
/// infinitesimal @c b_perturbation_vector.
///
/// The special cases are:
///
///  - The crossing point is equal to an end point of one or both segments.
///  - The segments are colinear.
///
/// @tparam b_perturbation_vector The infinitisemal offset to apply to @c b to resolve special cases.
/// @param a The first segment.
/// @param b The second segment.
/// @return The @c SegmentsCrossingPointParams of the crossing point if the two segments cross, or @c std::nullopt if
/// they don't cross.
template <PerturbationVector2 b_perturbation_vector>
inline std::optional<SegmentsCrossingPointParams> crossing_point_with_perturbation(Segment2 a, Segment2 b);

/// Writes @c segment to the output stream @c s.
///
/// @param s The output stream.
/// @param segment The @c Segment2 to write to the @c s.
/// @return A reference to the output stream.
std::ostream& operator<<(std::ostream& s, Segment2 segment);

} // namespace dida

#include "dida/segment2.inl"