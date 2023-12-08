#include "scene_view.hpp"

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include "dida/predicates.hpp"
#include "scene.hpp"
#include "scene_selection.hpp"

namespace dida::viz
{

namespace
{

/// The maximum L^inf distance in screen space between a point and the location of a mouse click for which the click is
/// considered to have hit that point.
constexpr float click_tolerance = 3;

/// Returns whether two points are within @c click_tolerance L^inf distance of each other.
bool points_within_click_tolerance(QPointF a, QPointF b)
{
  return std::abs(a.x() - b.x()) <= click_tolerance && std::abs(a.y() - b.y()) <= click_tolerance;
}

} // namespace

SceneView::SceneView(std::shared_ptr<VizScene> scene, std::shared_ptr<VizSceneSelection> selection)
    : scene_(scene), selection_(selection), tool_(SelectMoveTool{})
{
  QPalette palette;
  palette.setColor(QPalette::Window, Qt::white);
  setPalette(palette);
  setAutoFillBackground(true);

  QObject::connect(scene_.get(), &VizScene::data_changed, this, &SceneView::on_scene_data_changed);
  QObject::connect(selection_.get(), &VizSceneSelection::selection_changed, this, &SceneView::on_selection_changed);
}

void SceneView::switch_to_select_move_tool()
{
  tool_ = SelectMoveTool{};
}

void SceneView::switch_to_add_polygon_tool(bool add_convex_polygons)
{
  tool_ = AddPolygonTool{add_convex_polygons, nullptr};
}

void SceneView::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  paint_grid(painter);

  painter.setRenderHint(QPainter::Antialiasing);

  for (size_t primitive_index = 0; primitive_index < scene_->primitives().size(); primitive_index++)
  {
    const std::shared_ptr<VizPolygon>& polygon = scene_->primitives()[primitive_index];
    const ArrayView<const Point2> vertices = polygon->vertices();

    std::vector<QPointF> qt_vertices(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
      qt_vertices[i] = point_to_qt(vertices[i]);
    }

    if (is_polygon_being_drawn(polygon))
    {
      painter.drawPolyline(qt_vertices.data(), static_cast<int>(qt_vertices.size()));

      painter.setPen(Qt::DashLine);
      painter.drawLine(qt_vertices.front(), qt_vertices.back());
      painter.setPen(Qt::SolidLine);
    }
    else
    {
      painter.drawPolygon(qt_vertices.data(), static_cast<int>(qt_vertices.size()));
    }

    for (size_t vertex_index = 0; vertex_index < qt_vertices.size(); vertex_index++)
    {
      QPointF qt_vertex = qt_vertices[vertex_index];
      QColor color = selection_->is_vertex_selected(primitive_index, vertex_index) ? QColor(0, 64, 255) : Qt::black;
      painter.fillRect(QRect(qt_vertex.x() - 2, qt_vertex.y() - 2, 5, 5), color);
    }
  }
}

void SceneView::mousePressEvent(QMouseEvent* event)
{
  std::visit([this, event](auto& tool) { mouse_press_event_with_tool(event, tool); }, tool_);

  ZoomPanView::mousePressEvent(event);
}

void SceneView::mouseReleaseEvent(QMouseEvent* event)
{
  std::visit([this, event](auto& tool) { mouse_release_event_with_tool(event, tool); }, tool_);

  ZoomPanView::mouseReleaseEvent(event);
}

void SceneView::mouseMoveEvent(QMouseEvent* event)
{
  std::visit([this, event](auto& tool) { mouse_move_event_with_tool(event, tool); }, tool_);

  ZoomPanView::mouseMoveEvent(event);
}

void SceneView::on_scene_data_changed()
{
  update();
}

void SceneView::on_selection_changed()
{
  update();
}

void SceneView::mouse_press_event_with_tool(QMouseEvent* event, SelectMoveTool& tool)
{
}

void SceneView::mouse_release_event_with_tool(QMouseEvent* event, SelectMoveTool& tool)
{
}

void SceneView::mouse_move_event_with_tool(QMouseEvent* event, SelectMoveTool& tool)
{
}

void SceneView::mouse_press_event_with_tool(QMouseEvent* event, AddPolygonTool& tool)
{
  if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
  {
    Point2 vertex = point_from_qt(event->position());

    if (!tool.new_polygon)
    {
      tool.new_polygon = std::make_shared<VizPolygon>("polygon", std::vector<Point2>{vertex}, tool.add_convex_polygons);
      scene_->add_primitive(tool.new_polygon);
    }
    else
    {
      QPointF first_vertex = point_to_qt(tool.new_polygon->vertices().front());
      if (points_within_click_tolerance(first_vertex, event->position()))
      {
        tool.new_polygon = nullptr;

        // We've closed the polygon. No new vertices were added, so the scene hasn't changed, but since the closing edge
        // should now be drawn as a true edge, not a dashed edge, we still need a redraw.
        update();
      }
      else
      {
        tool.new_polygon->add_vertex(vertex);
      }
    }
  }
}

void SceneView::mouse_release_event_with_tool(QMouseEvent* event, AddPolygonTool& tool)
{
}

void SceneView::mouse_move_event_with_tool(QMouseEvent* event, AddPolygonTool& tool)
{
}

bool SceneView::is_polygon_being_drawn(const std::shared_ptr<VizPolygon>& polygon)
{
  if (AddPolygonTool* add_polygon_tool = std::get_if<AddPolygonTool>(&tool_))
  {
    return polygon == add_polygon_tool->new_polygon;
  }
  else
  {
    return false;
  }
}

} // namespace dida::viz