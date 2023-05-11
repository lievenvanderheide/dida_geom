#include "dida/assert.hpp"

namespace dida
{

Segment2::Segment2(Point2 start, Point2 end) : start_(start), end_(end)
{
  DIDA_ASSERT(start != end);
}

Segment2 Segment2::unsafe_from_endpoints(Point2 start, Point2 end)
{
  DIDA_DEBUG_ASSERT(start != end);

  Segment2 result;
  result.start_ = start;
  result.end_ = end;
  return result;
}

Point2 Segment2::start() const
{
  return start_;
}

Point2 Segment2::end() const
{
  return end_;
}

bool Segment2::operator==(const Segment2& b) const
{
  return start_ == b.start_ && end_ == b.end_;
}

bool Segment2::operator!=(const Segment2& b) const
{
  return start_ != b.start_ || end_ != b.end_;
}

SegmentsCrossingPointParams::SegmentsCrossingPointParams(ScalarDeg2 s_num, ScalarDeg2 t_num, ScalarDeg2 denom)
    : s_num_(s_num), t_num_(t_num), denom_(denom)
{
  DIDA_ASSERT(denom > 0);
  DIDA_ASSERT(s_num >= 0 && s_num <= denom);
  DIDA_ASSERT(t_num >= 0 && t_num <= denom);
}

inline ScalarDeg2 SegmentsCrossingPointParams::s_num() const
{
  return s_num_;
}

inline ScalarDeg2 SegmentsCrossingPointParams::t_num() const
{
  return t_num_;
}

inline ScalarDeg2 SegmentsCrossingPointParams::denom() const
{
  return denom_;
}

inline bool SegmentsCrossingPointParams::operator==(const SegmentsCrossingPointParams& b) const
{
  return s_num_ * b.denom_ == b.s_num_ * denom_ && t_num_ * b.denom_ == b.t_num_ * denom_;
}

inline bool SegmentsCrossingPointParams::operator!=(const SegmentsCrossingPointParams& b) const
{
  return !(*this == b);
}

template <PerturbationVector2 b_perturbation_vector>
inline std::optional<SegmentsCrossingPointParams> crossing_point_with_perturbation(Segment2 a, Segment2 b)
{
  // We want to find the 's' and 't' such that
  //
  //  a_start + t * a_dir = b_start + s * b_dir
  //
  //  cross(b_dir, a_start + s * a_dir - b_start) = 0
  //  s = -cross(b_dir, a_start - b_start) / cross(b_dir, a_dir)
  //    = cross(b_dir, b_start - a_start) / cross(b_dir, a_dir)
  //
  //  cross(a_dir, b_start + t * b_dir - a_start) = 0
  //  t = -cross(a_dir, b_start - a_start) / cross(a_dir, b_dir)
  //    = cross(a_dir, b_start - a_start) / cross(b_dir, a_dir)

  Vector2 a_dir = a.end() - a.start();
  Vector2 b_dir = b.end() - b.start();
  Vector2 b_start_to_a_start = b.start() - a.start();

  ScalarDeg2 s_num = cross(b_dir, b_start_to_a_start);
  ScalarDeg2 t_num = cross(a_dir, b_start_to_a_start);
  ScalarDeg2 denom = cross(b_dir, a_dir);

  if (denom == 0)
  {
    return std::nullopt;
  }

  bool signs_changed = denom < 0;
  if (signs_changed)
  {
    s_num = -s_num;
    t_num = -t_num;
    denom = -denom;
  }

  if (s_num < 0 || s_num > denom || t_num < 0 || t_num > denom)
  {
    return std::nullopt;
  }
  else if (s_num > 0 && s_num < denom && t_num > 0 && t_num < denom)
  {
    return SegmentsCrossingPointParams(s_num, t_num, denom);
  }
  else
  {
    // If 'signs_changed' is true, then we flip the two directions, to make sure that from now on, the two cross
    // products used to compute 's_num' and 't_num' immediately result in values of the correct sign.
    //
    //  s_num = cross(b_dir, b_start_to_a_start);
    //  t_num = cross(a_dir, b_start_to_a_start);
    //
    if (signs_changed)
    {
      a_dir = -a_dir;
      b_dir = -b_dir;
    }

    if (s_num == 0)
    {
      if (cross_is_negative(b_dir, b_perturbation_vector))
      {
        // After perturbation, s < 0.
        return std::nullopt;
      }
    }
    else if (s_num == denom)
    {
      if (cross_is_positive(b_dir, b_perturbation_vector))
      {
        // After perturbation, s > 1.
        return std::nullopt;
      }
    }

    if (t_num == 0)
    {
      if (cross_is_negative(a_dir, b_perturbation_vector))
      {
        // After perturbation, t < 0.
        return std::nullopt;
      }
    }
    else if (t_num == denom)
    {
      if (cross_is_positive(a_dir, b_perturbation_vector))
      {
        // After perturbation, t > 1.
        return std::nullopt;
      }
    }

    return SegmentsCrossingPointParams(s_num, t_num, denom);
  }
}

} // namespace dida