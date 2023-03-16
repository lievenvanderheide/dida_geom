#pragma once

#include <QtWidgets/QWidget>

#include "dida/viz/scene.hpp"

namespace dida::viz
{

/// The main view of the visualization tool, which shows the scene's geometry, and allows the user to edit it.
class SceneView : public QWidget
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
  void paint_grid(QPainter& painter) const;

  QPointF point_to_qt(Point2 point) const;

  std::shared_ptr<VizScene> scene_;

  double scale_;
  double translate_x_;
  double translate_y_;

  bool panning_;
  QPointF pan_previous_mouse_position_;
};

} // namespace dida::viz