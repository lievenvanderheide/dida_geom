#pragma once

#include <QtWidgets/QMainWindow>

#include "dida/viz/scene.hpp"

namespace dida::viz
{

class SceneView;

/// The main window of the Dida visualization tool.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /// Constructs a @c MainWindow with the given scene.
  MainWindow(std::shared_ptr<VizScene> scene);

private:
  void on_copy();

  void on_paste();

  std::shared_ptr<VizScene> scene_;

  SceneView* scene_view_;
};

} // namespace dida::viz