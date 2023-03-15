#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include "dida/viz/main_window.hpp"
#include "dida/viz/scene.hpp"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  std::vector<std::shared_ptr<dida::viz::VizPolygon>> polygons;
  std::shared_ptr<dida::viz::VizScene> scene = std::make_shared<dida::viz::VizScene>(std::move(polygons));

  dida::viz::MainWindow main_window(scene);
  main_window.resize(640, 480);

  main_window.showMaximized();

  return app.exec();
}