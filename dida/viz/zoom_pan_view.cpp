#include "dida/viz/zoom_pan_view.hpp"

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <iostream>

#include "dida/box2.hpp"
#include "dida/point2.hpp"

namespace dida::viz
{

/// The horizontal drag distance which corresponds to a doubling of @c scale_.
constexpr double scale_doubled_drag_distance = 100;
constexpr double min_grid_cell_size_in_view = 32;

ZoomPanView::ZoomPanView() : scale_(50), translate_x_(500), translate_y_(500), state_(State::idle)
{
}

void ZoomPanView::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton && event->modifiers() == Qt::AltModifier)
  {
    state_ = State::panning;
    previous_mouse_position_ = event->position();
  }
  else if (event->button() == Qt::RightButton && event->modifiers() == Qt::AltModifier)
  {
    state_ = State::zooming;
    previous_mouse_position_ = zoom_center_ = event->position();
  }
}

void ZoomPanView::mouseReleaseEvent(QMouseEvent* event)
{
  state_ = State::idle;
}

void ZoomPanView::mouseMoveEvent(QMouseEvent* event)
{
  switch (state_)
  {
  case State::idle:
    break;

  case State::panning:
  {
    QPointF position = event->position();
    translate_x_ += position.x() - previous_mouse_position_.x();
    translate_y_ += position.y() - previous_mouse_position_.y();
    previous_mouse_position_ = position;
    update();
  }
  break;

  case State::zooming:
  {
    // View location before zoom: pt * scale_ + translation_
    // View location after zoom: ((pt - from_view(zoom_center_)) * zoom + from_view(zoom_center_)) * scale_ +
    // translation_

    QPointF position = event->position();

    double zoom = std::pow(2, (position.x() - previous_mouse_position_.x()) / scale_doubled_drag_distance);
    translate_x_ += (1 - zoom) * (zoom_center_.x() - translate_x_);
    translate_y_ += (1 - zoom) * (zoom_center_.y() - translate_y_);
    scale_ *= zoom;

    previous_mouse_position_ = position;
    update();
  }
  break;
  }
}

QPointF ZoomPanView::point_to_view(Point2 point) const
{
  double x = static_cast<double>(point.x());
  double y = static_cast<double>(-point.y());
  return QPointF(x * scale_ + translate_x_, y * scale_ + translate_y_);
}

QRectF ZoomPanView::box_to_view(Box2 box) const
{
  const Vector2 diag = box.diag();
  double width = static_cast<double>(diag.x()) * scale_;
  double height = static_cast<double>(diag.y()) * scale_;
  return QRectF(point_to_view(Point2(box.min().x(), box.max().y())), QSizeF(width, height));
}

void ZoomPanView::paint_grid(QPainter& painter)
{
  QPen black_pen(Qt::black);
  QPen gray_pen(Qt::gray);

  // The distance in view space between adjacent grid lines should be the smallest power of 2 which is at least
  // min_grid_cell_size_in_view.
  //
  // scale_ * 2^n >= min_grid_cell_size_in_view
  //          2^n >= min_grid_cell_size_in_view / scale_

  double grid_cell_size = std::exp2(std::ceil(std::log2(min_grid_cell_size_in_view / scale_)));

  double left_x = ceil(-translate_x_ / (scale_ * grid_cell_size)) * grid_cell_size;
  double right_x = (width() - translate_x_) / scale_;
  for(double x = left_x; x <= right_x; x += grid_cell_size)
  {
    // Note: 'x' is always exact, so it's safe to compare it to 0.
    painter.setPen(x == 0 ? black_pen : gray_pen);
    painter.drawLine(x * scale_ + translate_x_, 0, x * scale_ + translate_x_, height());
  }

  double top_y = ceil(-translate_y_ / (scale_ * grid_cell_size)) * grid_cell_size;
  double bottom_y = (width() - translate_y_) / scale_;
  for(double y = top_y; y <= bottom_y; y += grid_cell_size)
  {
    // Note: 'y' is always exact, so it's safe to compare it to 0.
    painter.setPen(y == 0 ? black_pen : gray_pen);
    painter.drawLine(0, y * scale_ + translate_y_, width(), y * scale_ + translate_y_);
  }

  painter.setPen(QPen());
}

} // namespace dida::viz