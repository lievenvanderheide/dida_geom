#pragma once

#include <QtCore/QObject>
#include <set>

#include "scene.hpp"

namespace dida::viz
{

/// A class which manages the selection of the elements in a @c VizScene.
class VizSceneSelection : public QObject
{
  Q_OBJECT

public:
  /// Selects the vertex of the given primitive with the given index.
  void select_vertex(size_t primitive_index, size_t vertex_index);

  /// Selects the vertex of the given primitive with the given index.
  void deselect_vertex(size_t primitive_index, size_t vertex_index);

  /// Returns whether the vertex with the given indices is selected.
  bool is_vertex_selected(size_t primitive_index, size_t vertex_index);

Q_SIGNALS:
  /// Emitted when the selection changes.
  void selection_changed();

private:
  struct VertexKey
  {
    size_t primitive_index;

    size_t vertex_index;

    bool operator<(const VertexKey& b) const;
  };

  std::set<VertexKey> selected_vertices_;
};

} // namespace dida::viz