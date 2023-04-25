#pragma once

#include <QtWidgets/QWidget>

namespace dida
{
  class Point2;
  class Box2;
}

namespace dida::viz
{

/// A base class for views which support mouse zooming and panning.
///
///  - To zoom, the user has to hold the alt key, and then click and drag horizontally using the right mouse button.
///  - To pan, the user has to hold the alt key, and then click and drag using the right mouse button.
///
/// Derived classes should draw their own content in their @c paintEvent implementation. The geometry being painted
/// should be transformed manually from scene space to screen space using functions like @c point_to_view.
///
/// If @c mousePressEvent, @c mouseReleaseEvent and/or @c mouseMoveEvent are overridden in derived classes, then they
/// should chain to the respective base class implementation in their implementation.
class ZoomPanView : public QWidget
{
  Q_OBJECT

public:
  ZoomPanView();

protected:
  /// The implementation of @c mousePressEvent.
  ///
  /// @param event The event object.
  void mousePressEvent(QMouseEvent* event) override;

  /// The implementation of @c mouseReleaseEvent.
  ///
  /// @param event The event object.
  void mouseReleaseEvent(QMouseEvent* event) override;

  /// The implementation of @c mouseMoveEvent.
  ///
  /// @param event The event object.
  void mouseMoveEvent(QMouseEvent* event) override;

  /// Converts a point from scene to view space.
  ///
  /// @param point The point in scene space.
  /// @return The point in view space.
  QPointF point_to_view(Point2 point) const;

  /// Converts a box from scene to view space.
  ///
  /// @param box The box in scene space.
  /// @return The box in view space.
  QRectF box_to_view(Box2 box) const;

private:
  double scale_;
  double translate_x_;
  double translate_y_;

  enum class State
  {
    idle,
    panning,
    zooming,
  };

  State state_;
  QPointF previous_mouse_position_;
  QPointF zoom_center_;
};

} // namespace dida::viz
