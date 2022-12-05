#include "dida/viz/primitives_view.hpp"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

namespace dida::viz
{

PrimitivesView::PrimitivesView(std::shared_ptr<Scene> scene)
    : scene_(scene), scale_(50), translate_x_(500), translate_y_(500), panning_(false)
{
  QObject::connect(scene_.get(), &Scene::data_changed, this, &PrimitivesView::on_scene_data_changed);
}

void PrimitivesView::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  paint_grid(painter);

  for (const VizPolygon& polygon : scene_->primitives())
  {
    const ArrayView<const Point2> vertices = polygon.vertices();

    std::vector<QPointF> qt_vertices(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
      qt_vertices[i] = point_to_qt(vertices[i]);
    }

    painter.drawPolygon(qt_vertices.data(), static_cast<int>(qt_vertices.size()));

    for (QPointF point : qt_vertices)
    {
      painter.fillRect(QRect(point.x() - 2, point.y() - 2, 5, 5), Qt::black);
    }
  }
}

void PrimitivesView::paint_grid(QPainter& painter) const
{
  painter.drawLine(0, translate_y_, width(), translate_y_);
  painter.drawLine(translate_x_, 0, translate_x_, height());
}

QPointF PrimitivesView::point_to_qt(Point2 point) const
{
  double x = static_cast<double>(point.x());
  double y = static_cast<double>(-point.y());
  return QPointF(x * scale_ + translate_x_, y * scale_ + translate_y_);
}

void PrimitivesView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)
  {
    panning_ = true;
    pan_previous_mouse_position_ = event->position();
  }
}

void PrimitivesView::mouseReleaseEvent(QMouseEvent* event)
{
  panning_ = false;
}

void PrimitivesView::mouseMoveEvent(QMouseEvent* event)
{
  if(panning_)
  {
    QPointF position = event->position();
    translate_x_ += position.x() - pan_previous_mouse_position_.x();
    translate_y_ += position.y() - pan_previous_mouse_position_.y();
    pan_previous_mouse_position_ = position;
    update();
  }
}

void PrimitivesView::on_scene_data_changed()
{
  update();
}

} // namespace dida::viz