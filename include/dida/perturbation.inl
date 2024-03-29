namespace dida
{

constexpr PerturbationVector2 negate_perturbation_vector(PerturbationVector2 a)
{
  return a == PerturbationVector2::left_down ? PerturbationVector2::right_up : PerturbationVector2::left_down;
}

inline bool dot_is_positive(Vector2 a, PerturbationVector2 b)
{
  if (b == PerturbationVector2::left_down)
  {
    // Return the sign of dot(a, [-eps, -eps^2])
    return a.x() < 0 || (a.x() == 0 && a.y() < 0);
  }
  else
  {
    // Return the sign of cross(a, [eps, eps^2])
    return a.x() > 0 || (a.x() == 0 && a.y() > 0);
  }
}

inline bool dot_is_negative(Vector2 a, PerturbationVector2 b)
{
  if (b == PerturbationVector2::left_down)
  {
    // Return the sign of dot(a, [-eps, -eps^2])
    return a.x() > 0 || (a.x() == 0 && a.y() > 0);
  }
  else
  {
    // Return the sign of cross(a, [eps, eps^2])
    return a.x() < 0 || (a.x() == 0 && a.y() < 0);
  }
}

inline bool cross_is_positive(Vector2 a, PerturbationVector2 b)
{
  return dot_is_positive(left_perpendicular(a), b);
}

inline bool cross_is_negative(Vector2 a, PerturbationVector2 b)
{
  return dot_is_negative(left_perpendicular(a), b);
}

} // namespace dida