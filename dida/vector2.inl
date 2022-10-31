namespace dida
{

inline Vector2::Vector2(ScalarDeg1 x, ScalarDeg1 y) : coordinates_{x, y}
{
}

inline Vector2::Vector2(double x, double y) : Vector2(ScalarDeg1(x), ScalarDeg1(y))
{
}

inline Vector2 Vector2::from_grid_coordinates(int32_t x, int32_t y)
{
  return Vector2(ScalarDeg1::from_numerator(x), ScalarDeg1::from_numerator(y));
}

inline ScalarDeg1 Vector2::x() const
{
  return coordinates_[0];
}

inline ScalarDeg1 Vector2::y() const
{
  return coordinates_[1];
}

inline bool Vector2::operator==(Vector2 b) const
{
  return coordinates_[0] == b.coordinates_[0] && coordinates_[1] == b.coordinates_[1];
}

inline bool Vector2::operator!=(Vector2 b) const
{
  return coordinates_[0] != b.coordinates_[0] || coordinates_[1] != b.coordinates_[1];
}

inline Vector2 Vector2::operator+(Vector2 b) const
{
  return Vector2(x() + b.x(), y() + b.y());
}

inline Vector2 Vector2::operator-(Vector2 b) const
{
  return Vector2(x() - b.x(), y() - b.y());
}

inline Vector2 Vector2::operator-() const
{
  return Vector2(-x(), -y());
}

inline ScalarDeg2 dot(Vector2 a, Vector2 b)
{
  return a.x() * b.x() + a.y() * b.y();
}

inline ScalarDeg2 cross(Vector2 a, Vector2 b)
{
  return a.x() * b.y() - a.y() * b.x();
}

inline Vector2 left_perpendicular(Vector2 a)
{
  return Vector2(-a.y(), a.x());
}

inline Vector2 right_perpendicular(Vector2 a)
{
  return Vector2(a.y(), -a.x());
}

} // namespace dida