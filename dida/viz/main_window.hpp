#pragma once

#include <QtWidgets/QMainWindow>

#include "dida/viz/scene.hpp"

namespace dida::viz
{

/// The main window of the Dida visualization tool.
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /// Constructs a @c MainWindow with the given scene.
  MainWindow(std::shared_ptr<VizScene> scene);

private:
  void on_paste_primitive();

  std::shared_ptr<VizScene> scene_;
};

} // namespace dida::viz