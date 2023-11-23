#pragma once

#include "scene.hpp"
#include "zoom_pan_view.hpp"

namespace dida::viz
{

/// The main view of the visualization tool, which shows the scene's geometry, and allows the user to edit it.
class SceneView : public ZoomPanView
{
  Q_OBJECT

public:
  /// Constructs a @c SceneView with the given scene.
  ///
  /// @param scene The scene.
  SceneView(std::shared_ptr<VizScene> scene);

  /// Changes the active tool to the "Select/Move tool.
  void switch_to_select_move_tool();

  /// Changes the active tool to the "Add Polygon" tool.
  ///
  /// @param add_convex_polygons Whether the newly added polygons should be convex.
  void switch_to_add_polygon_tool(bool add_convex_polygons);
  
protected:
  void paintEvent(QPaintEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private Q_SLOTS:
  void on_scene_data_changed();

private:
  struct SelectMoveTool
  {
  };

  void mouse_press_event_with_tool(QMouseEvent* event, SelectMoveTool& tool);
  void mouse_release_event_with_tool(QMouseEvent* event, SelectMoveTool& tool);
  void mouse_move_event_with_tool(QMouseEvent* event, SelectMoveTool& tool);

  struct AddPolygonTool
  {
    bool add_convex_polygons;
    std::shared_ptr<VizPolygon> new_polygon;
  };

  void mouse_press_event_with_tool(QMouseEvent* event, AddPolygonTool& tool);
  void mouse_release_event_with_tool(QMouseEvent* event, AddPolygonTool& tool);
  void mouse_move_event_with_tool(QMouseEvent* event, AddPolygonTool& tool);

  bool is_polygon_being_drawn(const std::shared_ptr<VizPolygon>& polygon);

  std::shared_ptr<VizScene> scene_;

  std::variant<SelectMoveTool, AddPolygonTool> tool_;
};

} // namespace dida::viz