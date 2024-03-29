#include "scene.hpp"

#include <sstream>

#include "dida/convex_polygon2.hpp"
#include "dida/parser.hpp"
#include "dida/polygon2.hpp"

namespace dida::viz
{

void VizPolygon::add_vertex(Point2 vertex)
{
  size_t index = vertices_.size();
  will_add_vertex(index);
  vertices_.push_back(vertex);
  vertex_added(index);
  data_changed();
}

void VizPolygon::update_is_polygon_valid()
{
  is_polygon_valid_ =
      should_be_convex_ ? validate_convex_polygon_vertices(vertices_) : validate_polygon_vertices(vertices_);
}

std::shared_ptr<VizPolygon> parse_viz_polygon(Parser& parser)
{
  bool is_convex_polygon;
  if (parser.try_match("Polygon2"))
  {
    is_convex_polygon = false;
  }
  else if (parser.try_match("ConvexPolygon2"))
  {
    is_convex_polygon = true;
  }
  else
  {
    return nullptr;
  }

  if (!parser.skip_required_whitespace())
  {
    return nullptr;
  }

  std::optional<std::string_view> name = parser.parse_identifier();
  if (!name)
  {
    return nullptr;
  }

  parser.skip_optional_whitespace();
  std::optional<std::vector<Point2>> vertices = parser.parse_point2_vector();
  if (!vertices)
  {
    return nullptr;
  }

  parser.skip_optional_whitespace();
  if (!parser.match(';'))
  {
    return nullptr;
  }

  return std::make_shared<VizPolygon>(std::string(*name), *std::move(vertices), is_convex_polygon);
}

std::shared_ptr<VizPolygon> parse_viz_polygon(std::string_view string)
{
  Parser parser(string);
  std::shared_ptr<VizPolygon> result = parse_viz_polygon(parser);
  return parser.finished() ? result : nullptr;
}

std::ostream& operator<<(std::ostream& s, const VizPolygon& polygon)
{
  s << "ConvexPolygon2 " << polygon.name() << "{";

  const std::vector<Point2>& vertices = polygon.vertices();
  for (size_t i = 0; i < vertices.size(); i++)
  {
    s << vertices[i] << (i != vertices.size() - 1 ? ", " : "};");
  }

  return s;
}

void VizScene::add_primitive(std::shared_ptr<VizPolygon> polygon)
{
  QObject::connect(polygon.get(), &VizPolygon::data_changed, this, &VizScene::data_changed);

  size_t index = primitives_.size();
  will_add_primitive(index);
  primitives_.emplace_back(std::move(polygon));
  primitive_added(index);
  data_changed();
}

} // namespace dida::viz