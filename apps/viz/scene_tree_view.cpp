
#include "scene_tree_view.hpp"

#include <iomanip>
#include <sstream>

namespace dida::viz
{

SceneTreeView::SceneTreeView(std::shared_ptr<VizScene> scene)
{
  setHeaderHidden(true);

  SceneTreeModel* model = new SceneTreeModel(std::move(scene));
  setModel(model);

  QObject::connect(model, &SceneTreeModel::rowsInserted,
                   [this, model](QModelIndex parent, int first, int last)
                   {
                     for (int i = first; i <= last; i++)
                     {
                       expand(model->index(i, 0, parent));
                     }
                   });
}

namespace
{

#if UINTPTR_MAX == 0xffffffffffffffffu
constexpr int primitive_index_shift = 32;
constexpr quintptr element_index_mask = (static_cast<quintptr>(1) << primitive_index_shift) - 1;
#else
#error Only 64-bit platforms currently supported
#endif

/// The index of an item in a @c VizScene.
class ItemIndex
{
public:
  /// Constructs an @c ItemIndex from an item index packed in a @c quintptr.
  ///
  /// @param packed_index The packed index.
  /// @return The @c ItemIndex.
  static inline ItemIndex from_packed_index(quintptr packed_index);

  /// Returns this @c ItemIndex packed into a @c quintptr.
  ///
  /// @return The packed index.
  inline quintptr to_packed_index() const;

  /// Constructs a @c ItemIndex which refers to a primitive.
  ///
  /// @param primitive_idnex The index of the primitive.
  inline explicit ItemIndex(size_t primitive_index);

  /// Constructs a @c ItemIndex which refers to an element of a primitive.
  ///
  /// @param primitive_index The index of the primitive.
  /// @param element_index The index of the element within the primitive.
  inline ItemIndex(size_t primitive_index, size_t element_index);

  /// Returns whether this @c ItemIndex refers to a primitive or an element.
  ///
  /// @return True if the index refers to a primitive, false if it returns to an element.
  inline bool is_primitive() const;

  /// Returns the primitive index of this @c ItemIndex.
  ///
  /// @return The primitive index.
  inline size_t primitive_index() const;

  /// Returns the element index of this @c ItemIndex.
  ///
  /// @pre is_primitive() must return false.
  /// @return The element index.
  inline size_t element_index() const;

private:
  ItemIndex() = default;

  quintptr packed_index_;
};

ItemIndex ItemIndex::from_packed_index(quintptr packed_index)
{
  ItemIndex result;
  result.packed_index_ = packed_index;
  return result;
}

quintptr ItemIndex::to_packed_index() const
{
  return packed_index_;
}

ItemIndex::ItemIndex(size_t primitive_index)
{
  packed_index_ = (static_cast<quintptr>(primitive_index) << primitive_index_shift) + element_index_mask;
}

ItemIndex::ItemIndex(size_t primitive_index, size_t element_index)
{
  packed_index_ =
      (static_cast<quintptr>(primitive_index) << primitive_index_shift) + static_cast<quintptr>(element_index);
}

bool ItemIndex::is_primitive() const
{
  return (packed_index_ & element_index_mask) == element_index_mask;
}

size_t ItemIndex::primitive_index() const
{
  return static_cast<size_t>(packed_index_ >> primitive_index_shift);
}

size_t ItemIndex::element_index() const
{
  quintptr result = (packed_index_ & element_index_mask);
  DIDA_ASSERT(result != element_index_mask);
  return static_cast<size_t>(result);
}

} // namespace

SceneTreeModel::SceneTreeModel(std::shared_ptr<VizScene> scene) : scene_(std::move(scene))
{
  QObject::connect(scene_.get(), &VizScene::will_add_primitive, this, &SceneTreeModel::on_will_add_primitive);
  QObject::connect(scene_.get(), &VizScene::primitive_added, this, &SceneTreeModel::on_primitive_added);
}

QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    ItemIndex item_index(row);
    return createIndex(row, column, item_index.to_packed_index());
  }
  else
  {
    ItemIndex parent_index = ItemIndex::from_packed_index(parent.internalId());
    DIDA_ASSERT(parent_index.is_primitive());

    ItemIndex item_index(parent_index.primitive_index(), row);
    return createIndex(row, 0, item_index.to_packed_index());
  }
}

QVariant SceneTreeModel::data(const QModelIndex& index, int role) const
{
  ItemIndex item_index = ItemIndex::from_packed_index(index.internalId());
  const VizPolygon& polygon = *scene_->primitives()[item_index.primitive_index()];

  switch (role)
  {
  case Qt::DisplayRole:
    if (item_index.is_primitive())
    {
      return QString::fromUtf8(polygon.name().c_str());
    }
    else
    {
      Point2 vertex = polygon.vertices()[item_index.element_index()];

      std::stringstream s;
      s << std::fixed << std::setprecision(2);
      s << vertex;
      return QString::fromUtf8(s.str().c_str());
    }

  default:
    return QVariant();
  }
}

QModelIndex SceneTreeModel::parent(const QModelIndex& index) const
{
  ItemIndex item_index = ItemIndex::from_packed_index(index.internalId());

  if (item_index.is_primitive())
  {
    return QModelIndex();
  }
  else
  {
    size_t primitive_index = item_index.primitive_index();
    return createIndex(primitive_index, 0, ItemIndex(primitive_index).to_packed_index());
  }
}

int SceneTreeModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    return static_cast<int>(scene_->primitives().size());
  }

  ItemIndex item_index = ItemIndex::from_packed_index(parent.internalId());
  if (item_index.is_primitive())
  {
    return scene_->primitives()[item_index.primitive_index()]->vertices().size();
  }

  return 0;
}

int SceneTreeModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

void SceneTreeModel::on_will_add_primitive(size_t primitive_index)
{
  beginInsertRows(QModelIndex(), primitive_index, primitive_index);
}

void SceneTreeModel::on_primitive_added(size_t primitive_index)
{
  endInsertRows();

  VizPolygon* polygon = scene_->primitives()[primitive_index].get();
  QObject::connect(polygon, &VizPolygon::will_add_vertex,
                   [this, primitive_index](size_t vertex_index)
                   {
                     QModelIndex parent_index = index(primitive_index, 0, QModelIndex());
                     beginInsertRows(parent_index, vertex_index, vertex_index);
                   });
  QObject::connect(polygon, &VizPolygon::will_add_vertex, this, &SceneTreeModel::endInsertRows);
}

} // namespace dida::viz