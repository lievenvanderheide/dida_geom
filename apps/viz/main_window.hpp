#pragma once

#include <QtWidgets/QMainWindow>

namespace dida::viz
{

class SceneView;
class VizPolygon;
class VizScene;
class VizSceneSelection;

/// The main window of the Dida visualization tool.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /// Constructs a @c MainWindow with the given scene.
  MainWindow(std::shared_ptr<VizScene> scene, std::shared_ptr<VizSceneSelection> selection);
  ~MainWindow();

private:
  void on_copy();

  void on_paste();

  std::shared_ptr<VizScene> scene_;
  std::shared_ptr<VizSceneSelection> selection_;

  SceneView* scene_view_;
};

} // namespace dida::viz