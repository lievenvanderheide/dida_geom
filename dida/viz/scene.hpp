
#pragma once

#include <string>
#include <vector>
#include <QtCore/QObject>

#include "dida/array_view.hpp"
#include "dida/point2.hpp"

namespace dida
{

class Parser;

}

namespace dida::viz
{

class VizPolygon
{
public:
  VizPolygon(std::string name, std::vector<Point2> vertices, bool should_be_convex)
      : name_(std::move(name)), vertices_(std::move(vertices)), should_be_convex_(should_be_convex)
  {
    update_is_valid();
  }

  ArrayView<const Point2> vertices() const
  {
    return vertices_;
  }

private:
  void update_is_valid();

  std::string name_;
  std::vector<Point2> vertices_;
  bool should_be_convex_;
  bool is_valid_;
};

std::optional<VizPolygon> parse_viz_polygon(Parser& parser);
std::optional<VizPolygon> parse_viz_polygon(std::string_view string);

class Scene : public QObject
{
  Q_OBJECT

public:
  Scene(std::vector<VizPolygon> primitives) : primitives_(std::move(primitives))
  {
  }

  ArrayView<const VizPolygon> primitives()
  {
    return primitives_;
  }

  void add_primitive(VizPolygon polygon)
  {
    primitives_.emplace_back(std::move(polygon));
    data_changed();
  }

Q_SIGNALS:
  void data_changed();

private:
  std::vector<VizPolygon> primitives_;
};

} // namespace dida::viz