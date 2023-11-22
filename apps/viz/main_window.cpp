#include "main_window.hpp"

#include <QtGui/QActionGroup>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QShortcut>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolBar>

#include <iomanip>
#include <sstream>

#include "scene_tree_view.hpp"
#include "scene_view.hpp"

namespace dida::viz
{

MainWindow::MainWindow(std::shared_ptr<VizScene> scene) : scene_(std::move(scene))
{
  scene_view_ = new SceneView(scene_);
  setCentralWidget(scene_view_);

  SceneTreeView* tree_view = new SceneTreeView(scene_);
  QDockWidget* tree_view_dock_widget = new QDockWidget();
  tree_view_dock_widget->setWidget(tree_view);
  addDockWidget(Qt::LeftDockWidgetArea, tree_view_dock_widget);

  QMenu* edit_menu = menuBar()->addMenu("&Edit");

  QAction* copy_action = edit_menu->addAction("&Copy", this, &MainWindow::on_copy);
  copy_action->setShortcut(QKeySequence::Copy);

  QAction* paste_action = edit_menu->addAction("&Paste", this, &MainWindow::on_paste);
  paste_action->setShortcut(QKeySequence::Paste);

  QToolBar* toolbar = new QToolBar();
  addToolBar(Qt::RightToolBarArea, toolbar);

  QActionGroup* tool_action_group = new QActionGroup(this);

  QAction* select_move_tool_action = new QAction("Select Move", tool_action_group);
  select_move_tool_action->setCheckable(true);
  select_move_tool_action->setChecked(true);
  QObject::connect(select_move_tool_action, &QAction::triggered, scene_view_, &SceneView::switch_to_select_move_tool);
  toolbar->addAction(select_move_tool_action);

  QAction* add_polygon_tool_action = new QAction("Add Polygon", tool_action_group);
  add_polygon_tool_action->setCheckable(true);
  QObject::connect(add_polygon_tool_action, &QAction::triggered,
                   [this]() { scene_view_->switch_to_add_polygon_tool(false); });
  toolbar->addAction(add_polygon_tool_action);

  QAction* add_convex_polygon_tool_action = new QAction("Add Convex Polygon", tool_action_group);
  add_convex_polygon_tool_action->setCheckable(true);
  QObject::connect(add_convex_polygon_tool_action, &QAction::triggered,
                   [this]() { scene_view_->switch_to_add_polygon_tool(true); });
  toolbar->addAction(add_convex_polygon_tool_action);
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