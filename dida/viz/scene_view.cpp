#include "dida/viz/scene_view.hpp"

#include <QtGui/QPainter>

#include "dida/box2.hpp"

namespace dida::viz
{

void SceneView::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  for (int y = -5; y <= 5; y++)
  {
    for (int x = -5; x <= 5; x++)
    {
      QPointF point = point_to_view(Point2(static_cast<double>(x), static_cast<double>(y)));
      painter.fillRect(QRect(point.x() - 2, point.y() - 2, 5, 5), Qt::black);
    }
  }

  painter.drawRect(box_to_view(Box2(Point2(-11.5, -10.5), Point2(6.5, 5.5))));
}

} // namespace dida::viz