#include "dida/viz/scene.hpp"

#include "dida/convex_polygon2.hpp"
#include "dida/parser.hpp"

namespace dida::viz
{

void VizPolygon::update_is_valid()
{
  DIDA_ASSERT(should_be_convex_);
  is_valid_ = validate_convex_polygon_vertices(vertices_);
}

std::optional<VizPolygon> parse_viz_polygon(Parser& parser)
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
    return std::nullopt;
  }

  parser.skip_required_whitespace();

  std::optional<std::string_view> name = parser.parse_identifier();
  if (!name)
  {
    return std::nullopt;
  }

  parser.skip_optional_whitespace();
  std::optional<std::vector<Point2>> vertices = parser.parse_point2_vector();
  if (!vertices)
  {
    return std::nullopt;
  }

  return VizPolygon(std::string(*name), *std::move(vertices), is_convex_polygon);
}

std::optional<VizPolygon> parse_viz_polygon(std::string_view string)
{
  Parser parser(string);
  std::optional<VizPolygon> result = parse_viz_polygon(parser);
  return parser.finished() ? result : std::nullopt;
}

} // namespace dida::viz