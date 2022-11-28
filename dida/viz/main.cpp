#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include <iostream>

#include "dida/viz/main_window.hpp"
#include "dida/viz/scene.hpp"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  dida::viz::VizPolygon polygon = *dida::viz::parse_viz_polygon("ConvexPolygon2 polygon {"
                                                                "{-1.4,-1.68},"
                                                                "{-4.44,-3.28},"
                                                                "{-2.64,0.82},"
                                                                "{-3.62,2.6},"
                                                                "{4.28,0.9},"
                                                                "{1.18,-4.12}"
                                                                "}");

  std::vector<dida::viz::VizPolygon> polygons{std::move(polygon)};
  std::shared_ptr<dida::viz::Scene> scene = std::make_shared<dida::viz::Scene>(std::move(polygons));

  dida::viz::MainWindow main_window;
  main_window.show();

  return app.exec();
}