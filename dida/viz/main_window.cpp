#include "dida/viz/main_window.hpp"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QShortcut>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>

#include <iomanip>
#include <sstream>

#include "dida/viz/scene_tree_view.hpp"
#include "dida/viz/scene_view.hpp"

namespace dida::viz
{

MainWindow::MainWindow(std::shared_ptr<VizScene> scene) : scene_(std::move(scene))
{
  QMenu* edit_menu = menuBar()->addMenu("&Edit");

  QAction* copy_action = edit_menu->addAction("&Copy", this, &MainWindow::on_copy);
  copy_action->setShortcut(QKeySequence::Copy);

  QAction* paste_action = edit_menu->addAction("&Paste", this, &MainWindow::on_paste);
  paste_action->setShortcut(QKeySequence::Paste);

  scene_view_ = new SceneView(scene_);
  setCentralWidget(scene_view_);

  SceneTreeView* tree_view = new SceneTreeView(scene_);
  QDockWidget* tree_view_dock_widget = new QDockWidget();
  tree_view_dock_widget->setWidget(tree_view);
  addDockWidget(Qt::LeftDockWidgetArea, tree_view_dock_widget);
}

void MainWindow::on_copy()
{
  std::stringstream s;
  s << std::fixed << std::setprecision(2);

  for (const std::shared_ptr<VizPolygon>& primitive : scene_->primitives())
  {
    s << *primitive << std::endl;
  }

  QGuiApplication::clipboard()->setText(QString::fromUtf8(s.str().c_str()));
}

void MainWindow::on_paste()
{
  QString clipboard_text = QGuiApplication::clipboard()->text();
  QByteArray clipboard_text_utf8 = clipboard_text.toUtf8();

  std::shared_ptr<VizPolygon> polygon =
      parse_viz_polygon(std::string_view(clipboard_text_utf8.data(), clipboard_text_utf8.size()));
  if (!polygon)
  {
    QMessageBox message_box;
    message_box.setText("Failed to parse primitive");
    message_box.exec();
    return;
  }

  scene_->add_primitive(std::move(polygon));
}

} // namespace dida::viz