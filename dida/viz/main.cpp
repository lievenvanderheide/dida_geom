#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include <iostream>

#include "dida/viz/main_window.hpp"
#include "dida/viz/scene.hpp"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  std::vector<dida::viz::VizPolygon> polygons;
  std::shared_ptr<dida::viz::Scene> scene = std::make_shared<dida::viz::Scene>(std::move(polygons));

  dida::viz::MainWindow main_window(scene);

  main_window.show();

  return app.exec();
}