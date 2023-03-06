#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QTreeView>

#include "dida/viz/scene.hpp"

namespace dida::viz
{

class PrimitivesTreeView : public QTreeView
{
  Q_OBJECT

public:
  PrimitivesTreeView(std::shared_ptr<Scene> scene);

private:
  class SceneTreeModel : public QAbstractItemModel
  {
  public:
    SceneTreeModel(std::shared_ptr<Scene> scene);

    QVariant data(const QModelIndex& index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  private:
    void on_begin_add_primitive(size_t index);
    void on_primitive_added(size_t index);

    std::shared_ptr<Scene> scene_;
  };
};

} // namespace dida::viz