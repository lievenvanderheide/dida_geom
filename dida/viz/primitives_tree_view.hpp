#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QTreeView>

#include "dida/viz/scene.hpp"

namespace dida::viz
{

/// A tree view which shows the contents of a @c VizScene.
class VizSceneTreeView : public QTreeView
{
  Q_OBJECT

public:
  /// Constructs a @c VizSceneTreeView with the given scene.
  ///
  /// @param scene The @c VizScene to show in the tree view.
  VizSceneTreeView(std::shared_ptr<VizScene> scene);
};

/// A @c QAbstractItemModel wrapper around @c VizScene.
class PrimitivesTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  /// Constructs a @c PrimitivesTreeModel with the given scene.
  ///
  /// @param scene The @c VizScene to wrap.
  PrimitivesTreeModel(std::shared_ptr<VizScene> scene);

  /// Implements the @c index function of @c QAbstractItemModel.
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  
  /// Implements the @c data function of @c QAbstractItemModel.
  ///
  /// If role == PrimitivesTreeModel, then the return value is
  ///  - The name of a primitive if @c index refers to a @c primitive.
  ///  - The c-style representation of the coordinates of @c index refers to a vertex.
  ///
  /// In all other cases, an empty @c QVariant is returned.
  ///
  /// @param index The index of the element for which to return data.
  /// @param role The role for which to return data.
  /// @return A @c QVariant containing the data.
  QVariant data(const QModelIndex& index, int role) const override;

  /// Implements the @c parent function of @c QAbstractItemModel.
  QModelIndex parent(const QModelIndex& index) const override;

  /// Implements the @c rowCount function of @c QAbstractItemModel.
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  /// Implements the @c columnCount function of @c QAbstractItemModel.
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
  void on_will_add_primitive(size_t index);
  void on_primitive_added(size_t index);

  std::shared_ptr<VizScene> scene_;
};

} // namespace dida::viz