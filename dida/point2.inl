namespace dida
{

inline Point2::Point2(ScalarDeg1 x, ScalarDeg1 y) : position_(x, y)
{
}

inline Point2::Point2(double x, double y) : position_(x, y)
{
}

inline Point2::Point2(Vector2 position) : position_(position)
{
}

inline Point2::operator Vector2() const
{
  return position_;
}

inline ScalarDeg1 Point2::x() const
{
  return position_.x();
}

inline ScalarDeg1 Point2::y() const
{
  return position_.y();
}

inline bool Point2::operator==(Point2 b) const
{
  return position_ == b.position_;
}

inline bool Point2::operator!=(Point2 b) const
{
  return position_ != b.position_;
}

inline Vector2 Point2::operator-(const Point2 b) const
{
  return position_ - b.position_;
}

inline Point2 Point2::operator+(Vector2 b) const
{
  return Point2(position_ + b);
}

inline Point2 Point2::operator-(Vector2 b) const
{
  return Point2(position_ - b);
}

inline bool lex_less_than(Point2 a, Point2 b)
{
  return a.x() < b.x() || (a.x() == b.x() && a.y() < b.y());
}

inline bool lex_greater_than(Point2 a, Point2 b)
{
  return a.x() > b.x() || (a.x() == b.x() && a.y() > b.y());
}

inline std::ostream& operator<<(std::ostream& s, Point2 v)
{
  return s << static_cast<Vector2>(v);
}

} // namespace dida