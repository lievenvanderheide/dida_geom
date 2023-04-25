#pragma once

#include "dida/viz/zoom_pan_view.hpp"

namespace dida::viz
{

class SceneView : public ZoomPanView
{
  Q_OBJECT

protected:
  void paintEvent(QPaintEvent* event);
};

} // namespace dida::viz