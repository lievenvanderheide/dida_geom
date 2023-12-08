#include "scene_selection.hpp"

namespace dida::viz
{

void VizSceneSelection::select_vertex(size_t primitive_index, size_t vertex_index)
{
  if (selected_vertices_.insert(VertexKey{primitive_index, vertex_index}).second)
  {
    selection_changed();
  }
}

void VizSceneSelection::deselect_vertex(size_t primitive_index, size_t vertex_index)
{
  if (selected_vertices_.erase(VertexKey{primitive_index, vertex_index}))
  {
    selection_changed();
  }
}

bool VizSceneSelection::is_vertex_selected(size_t primitive_index, size_t vertex_index)
{
  return selected_vertices_.find(VertexKey{primitive_index, vertex_index}) != selected_vertices_.end();
}

bool VizSceneSelection::VertexKey::operator<(const VertexKey& b) const
{
  return primitive_index < b.primitive_index || (primitive_index == b.primitive_index && vertex_index < b.vertex_index);
}

} // namespace dida::viz