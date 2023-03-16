#include "dida/viz/scene_view.hpp"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

namespace dida::viz
{

SceneView::SceneView(std::shared_ptr<VizScene> scene)
    : scene_(scene), scale_(50), translate_x_(500), translate_y_(500), panning_(false)
{
  QPalette palette;
  palette.setColor(QPalette::Window, Qt::white);
  setPalette(palette);
  setAutoFillBackground(true);

  QObject::connect(scene_.get(), &VizScene::data_changed, this, &SceneView::on_scene_data_changed);
}

void SceneView::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  paint_grid(painter);

  for (const std::shared_ptr<VizPolygon>& polygon : scene_->primitives())
  {
    const ArrayView<const Point2> vertices = polygon->vertices();

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

void SceneView::paint_grid(QPainter& painter) const
{
  painter.drawLine(0, translate_y_, width(), translate_y_);
  painter.drawLine(translate_x_, 0, translate_x_, height());
}

QPointF SceneView::point_to_qt(Point2 point) const
{
  double x = static_cast<double>(point.x());
  double y = static_cast<double>(-point.y());
  return QPointF(x * scale_ + translate_x_, y * scale_ + translate_y_);
}

void SceneView::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)
  {
    panning_ = true;
    pan_previous_mouse_position_ = event->position();
  }
}

void SceneView::mouseReleaseEvent(QMouseEvent* event)
{
  panning_ = false;
}

void SceneView::mouseMoveEvent(QMouseEvent* event)
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

void SceneView::on_scene_data_changed()
{
  update();
}

} // namespace dida::viz