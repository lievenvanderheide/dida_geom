namespace dida
{

Box2::Box2(Point2 min, Point2 max) : min_(min), max_(max)
{
}

Point2 Box2::min() const
{
  return min_;
}

Point2 Box2::max() const
{
  return max_;
}

Vector2 Box2::diag() const
{
  return max_ - min_;
}

} // namespace dida