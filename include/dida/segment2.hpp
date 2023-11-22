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
  inline Segment2(Point2 start, Point2 end);

  /// Constructs a segment with the given start and end points.
  ///
  /// @pre The two end points must be distinct. It's verified using a @c DIDA_DEBUG_ASSERT that this is the case.
  static inline Segment2 unsafe_from_endpoints(Point2 start, Point2 end);

  /// Returns the start point of this @c Segment2.
  inline Point2 start() const;

  /// Returns the end point of this @c Segment2.
  inline Point2 end() const;

  /// Returns the direction vector <tt>end() - start()</tt> of this @c Segment2.
  inline Vector2 direction() const;

  /// Compares two segments for equality.
  inline bool operator==(const Segment2& b) const;

  /// Compares two segments for inequality.
  inline bool operator!=(const Segment2& b) const;

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
  inline SegmentsCrossingPointParams(ScalarDeg2 s_num, ScalarDeg2 t_num, ScalarDeg2 denom);

  /// Returns the numerator of the @c s parameter.
  inline ScalarDeg2 s_num() const;

  /// Returns the numerator of the @c t parameter.
  inline ScalarDeg2 t_num() const;

  /// Returns the denominator of both parameters.
  inline ScalarDeg2 denom() const;

  /// Compares two @c SegmentsCrossingPointParams instances for equality.
  ///
  /// Two @c SegmentsCrossingPointParams are considered equal iff both have to the same values for <tt>s = s_num /
  /// denom</tt> and <tt>t = t_num / denom</tt>, even if the values of @c s_num, @c t_num and @c denom are different.
  inline bool operator==(const SegmentsCrossingPointParams& b) const;

  /// Compares two @c SegmentsCrossingPointParams instances for inequality.
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
template <PerturbationVector2 b_perturbation_vector>
inline std::optional<SegmentsCrossingPointParams> crossing_point_with_perturbation(Segment2 a, Segment2 b);

/// Writes @c segment to the output stream @c s.
std::ostream& operator<<(std::ostream& s, Segment2 segment);

} // namespace dida

#include "dida/segment2.inl"