#pragma once

#include "dida/viz/scene.hpp"
#include "dida/viz/zoom_pan_view.hpp"

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
  
protected:
  void paintEvent(QPaintEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private Q_SLOTS:
  void on_scene_data_changed();

private:
  struct AddPolygonTool
  {
    std::shared_ptr<VizPolygon> new_polygon;
  };

  void mouse_press_event_with_tool(QMouseEvent* event, AddPolygonTool& tool);
  void mouse_release_event_with_tool(QMouseEvent* event, AddPolygonTool& tool);
  void mouse_move_event_with_tool(QMouseEvent* event, AddPolygonTool& tool);

  bool is_polygon_being_drawn(const std::shared_ptr<VizPolygon>& polygon);

  std::shared_ptr<VizScene> scene_;

  std::variant<AddPolygonTool> tool_;
};

} // namespace dida::viz