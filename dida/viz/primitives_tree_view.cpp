
#include "dida/viz/primitives_tree_view.hpp"

#include <sstream>

namespace dida::viz
{

#if UINTPTR_MAX == 0xffffffffffffffffu
constexpr int primitive_index_shift = 32;
constexpr quintptr subobject_index_mask = (static_cast<quintptr>(1) << primitive_index_shift) - 1;
#else
#error Only 64-bit platforms currently   supported
#endif

namespace
{

bool packed_index_is_primitive(quintptr packed_index)
{
  return (packed_index & subobject_index_mask) == subobject_index_mask;
}

size_t packed_index_primitive_index(quintptr packed_index)
{
  return static_cast<size_t>(packed_index >> primitive_index_shift);
}

size_t packed_index_subobject_index(quintptr packed_index)
{
  quintptr result = (packed_index & subobject_index_mask);
  DIDA_ASSERT(result != subobject_index_mask);
  return static_cast<size_t>(result);
}

}

PrimitivesTreeView::PrimitivesTreeView(std::shared_ptr<Scene> scene)
{
  setHeaderHidden(true);
  
  SceneTreeModel* model = new SceneTreeModel(std::move(scene));
  setModel(model);
}

PrimitivesTreeView::SceneTreeModel::SceneTreeModel(std::shared_ptr<Scene> scene) : scene_(std::move(scene))
{
  QObject::connect(scene_.get(), &Scene::begin_add_primitive, this, &SceneTreeModel::on_begin_add_primitive);
  QObject::connect(scene_.get(), &Scene::primitive_added, this, &SceneTreeModel::on_primitive_added);
}

QVariant PrimitivesTreeView::SceneTreeModel::data(const QModelIndex& index, int role) const
{
  quintptr packed_index = index.internalId();

  switch(role)
  {
  case Qt::DisplayRole:
    if(packed_index_is_primitive(packed_index))
    {
      const VizPolygon& polygon = scene_->primitives()[packed_index_primitive_index(packed_index)];
      return QString::fromUtf8(polygon.name().c_str());
    }
    else
    {
      const VizPolygon& polygon = scene_->primitives()[packed_index_primitive_index(packed_index)];
      Point2 vertex = polygon.vertices()[index.row()];

      std::stringstream s;
      s << vertex;
      return QString::fromUtf8(s.str().c_str());
    }

  default:
    return QVariant();
  }
}

QModelIndex PrimitivesTreeView::SceneTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    quintptr indices_packed = (static_cast<quintptr>(row) << primitive_index_shift) + subobject_index_mask;
    return createIndex(row, column, indices_packed);
  }
  else
  {
    DIDA_ASSERT(packed_index_is_primitive(parent.internalId()));

    quintptr indices_packed = (parent.internalId() & ~subobject_index_mask) + static_cast<quintptr>(row);
    DIDA_ASSERT(!packed_index_is_primitive(indices_packed));
    return createIndex(row, 0, indices_packed);
  }
}

QModelIndex PrimitivesTreeView ::SceneTreeModel::parent(const QModelIndex& _index) const
{
  if(packed_index_is_primitive(_index.internalId()))
  {
    return QModelIndex();
  }
  else
  {
    size_t primitive_index = packed_index_primitive_index(_index.internalId());
    quintptr indices_packed = _index.internalId() | subobject_index_mask;
    return createIndex(primitive_index, 0, indices_packed);
  }
}

int PrimitivesTreeView::SceneTreeModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    return static_cast<int>(scene_->primitives().size());
  }
  else if(packed_index_is_primitive(parent.internalId()))
  {
    size_t primitive_index = packed_index_primitive_index(parent.internalId());
    return scene_->primitives()[primitive_index].vertices().size();
  }

  return 0;
}

int PrimitivesTreeView::SceneTreeModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

void PrimitivesTreeView::SceneTreeModel::on_begin_add_primitive(size_t index)
{
  beginInsertRows(QModelIndex(), index, index);
}

void PrimitivesTreeView::SceneTreeModel::on_primitive_added(size_t index)
{
  endInsertRows();
}

} // namespace dida::viz