#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include "main_window.hpp"
#include "scene.hpp"
#include "scene_selection.hpp"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  std::vector<std::shared_ptr<dida::viz::VizPolygon>> polygons;
  std::shared_ptr<dida::viz::VizScene> scene = std::make_shared<dida::viz::VizScene>(std::move(polygons));
  std::shared_ptr<dida::viz::VizSceneSelection> selection = std::make_shared<dida::viz::VizSceneSelection>();

  dida::viz::MainWindow main_window(scene, selection);
  main_window.resize(640, 480);

  main_window.showMaximized();

  return app.exec();
}