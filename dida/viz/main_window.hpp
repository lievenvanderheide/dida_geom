#pragma once

#include <QtWidgets/QMainWindow>

#include "dida/viz/scene.hpp"

namespace dida::viz
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<Scene> scene);

private Q_SLOTS:
    void on_add_primitive_from_text();

private:
    std::shared_ptr<Scene> scene_;
};

}