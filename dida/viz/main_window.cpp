#include "dida/viz/main_window.hpp"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>

#include "dida/viz/primitives_view.hpp"

namespace dida::viz
{

MainWindow::MainWindow(std::shared_ptr<Scene> scene)
    : scene_(scene)
{
  dida::viz::PrimitivesView* primitives_view = new dida::viz::PrimitivesView(scene);
  setCentralWidget(primitives_view);

  QMenu* add_menu = menuBar()->addMenu("Add");
  add_menu->addAction("Add Primitive from Text", this, &MainWindow::on_add_primitive_from_text);
}

void MainWindow::on_add_primitive_from_text()
{
  QString clipboard_text = QGuiApplication::clipboard()->text();
  QByteArray clipboard_text_utf8 = clipboard_text.toUtf8();

  std::optional<VizPolygon> polygon =
      parse_viz_polygon(std::string_view(clipboard_text_utf8.data(), clipboard_text_utf8.size()));
  if (!polygon)
  {
    QMessageBox message_box;
    message_box.setText("Failed to parse primitive");
    message_box.exec();
    return;
  }

  scene_->add_primitive(*std::move(polygon));
}

} // namespace dida::viz