#include "dida/viz/main_window.hpp"

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QShortcut>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenuBar>

namespace dida::viz
{

MainWindow::MainWindow(std::shared_ptr<VizScene> scene) : scene_(scene)
{
  QMenu* edit_menu = menuBar()->addMenu("&Edit");

  QAction* paste_action =
      edit_menu->addAction("&Paste", this, &MainWindow::on_paste_primitive);
  paste_action->setShortcut(QKeySequence::Paste);
}

void MainWindow::on_paste_primitive()
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