namespace dida::viz
{

VizPolygon::VizPolygon(std::string name, std::vector<Point2> vertices, bool should_be_convex)
    : name_(std::move(name)), vertices_(std::move(vertices)), should_be_convex_(should_be_convex)
{
  update_is_polygon_valid();
}

const std::string& VizPolygon::name() const
{
  return name_;
}

const std::vector<Point2>& VizPolygon::vertices() const
{
  return vertices_;
}

bool VizPolygon::should_be_convex() const
{
  return should_be_convex_;
}

bool VizPolygon::is_polygon_valid() const
{
  return is_polygon_valid_;
}

VizScene::VizScene(std::vector<std::shared_ptr<VizPolygon>> primitives) : primitives_(std::move(primitives))
{
}

const std::vector<std::shared_ptr<VizPolygon>>& VizScene::primitives()
{
  return primitives_;
}

void VizScene::add_primitive(std::shared_ptr<VizPolygon> polygon)
{
  primitives_.emplace_back(std::move(polygon));
  data_changed();
}

} // namespace dida::viz