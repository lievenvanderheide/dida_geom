#include "dida/detail/vertical_decomposition/triangulate.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

struct TriangulateState
{
  /// The vertices of the polygon we're triangulating.
  VerticesView vertices;

  /// An iterator pointing to the next free element in the result vector.
  std::vector<Triangle2>::iterator result_it;

  /// The next vertex on the lower boundary of the current monotone channel.
  VertexIt lower_vertex_it;

  /// The next vertex on the upper boundary of the current monotone channel.
  VertexIt upper_vertex_it;

  /// The previous node.
  const Node* prev_node;

  /// The next node.
  const Node* next_node;

  /// True iff the current front is on the lower boundary.
  bool front_on_lower_boundary;

  /// The vertices of the concave front of the already triangulated part. @c front_vertices.front() is the leftmost
  /// vertex, @c front_vertices.back() the rightmost vertex.
  ///
  /// If @c front_on_lower_boundary is @c true, then the edges in this front are facing upwards, if @c
  /// front_on_lower_boundary is @c false then the edges are facing downwards (though note that the winding is opposite
  /// to the usual winding in this case, because @c front_vertices.front() is still the leftmost vertex).
  std::vector<Point2> front_vertices;
};

/// Triangulates the monotone channel starting from the given left leaf.
void triangulate_monotone_channel_from_left_leaf(TriangulateState& state, const Node* left_leaf_node);

/// Triangulates the "secondary" monotone channel of a right branch, when the secondary channel is the lower branch.
///
/// A secondary channel whcih goes into the lower branch exists if the vertex immediately before the branch is on the
/// lower boundary.
void triangulate_monotone_channel_from_lower_right_branch(TriangulateState& state, const Node* node);

/// Triangulates the "secondary" monotone channel of a right branch, when the secondary channel is the upper branch.
///
/// A secondary channel whcih goes into the upper branch exists if the vertex immediately before the branch is on the
/// upper boundary.
void triangulate_monotone_channel_from_upper_right_branch(TriangulateState& state, const Node* node);

/// Implements the main loop which triangulates a channel. This function is called by the above 3 functions.
void triangulate_monotone_channel(TriangulateState& state);

/// Handles the case of @c triangulate_monotone_channel when a left branch is encountered.
bool triangulate_handle_left_branch(TriangulateState& state);

/// Handles the case of @c triangulate_monotone_channel when a right branch is encountered.
void triangulate_handle_right_branch(TriangulateState& state);

/// Handles the case of @c triangulate_monotone_channel when a vertex on the lower boundary of the current monotone
/// channel is encountered.
void triangulate_handle_lower_vertex(TriangulateState& state, Point2 vertex);

/// Handles the case of @c triangulate_monotone_channel when a vertex on the upper boundary of the current monotone
/// channel is encountered.
void triangulate_handle_upper_vertex(TriangulateState& state, Point2 vertex);

/// Handles the case of @c triangulate_monotone_channel when the last vertex of a channel is encountered.
void triangulate_handle_last_vertex(TriangulateState& state, Point2 vertex);

/// Returns whether the triangle formed by the given 3 vertices has counter clockwise winding.
bool is_counter_clockwise(Point2 v0, Point2 v1, Point2 v2);

void triangulate_monotone_channel_from_left_leaf(TriangulateState& state, const Node* left_leaf_node)
{
  DIDA_DEBUG_ASSERT(left_leaf_node->type == NodeType::leaf);
  DIDA_DEBUG_ASSERT(left_leaf_node->direction == HorizontalDirection::left);

  state.lower_vertex_it = next_cyclic(state.vertices, left_leaf_node->vertex_it);
  state.upper_vertex_it = prev_cyclic(state.vertices, left_leaf_node->vertex_it);
  state.prev_node = left_leaf_node;
  state.next_node = left_leaf_node->neighbors[0];

  state.front_vertices.resize(1);
  state.front_vertices[0] = *left_leaf_node->vertex_it;

  triangulate_monotone_channel(state);
}

void triangulate_monotone_channel_from_lower_right_branch(TriangulateState& state, const Node* node)
{
  state.lower_vertex_it = node->lower_opp_edge.end_vertex_it;
  state.upper_vertex_it = node->vertex_it;

  state.prev_node = node;
  state.next_node = node->neighbors[1];

  state.front_vertices.resize(1);
  state.front_vertices[0] = *node->lower_opp_edge.start_vertex_it;
  state.front_on_lower_boundary = false;

  triangulate_monotone_channel(state);
}

void triangulate_monotone_channel_from_upper_right_branch(TriangulateState& state, const Node* node)
{
  state.lower_vertex_it = node->vertex_it;
  state.upper_vertex_it = node->upper_opp_edge.start_vertex_it;

  state.prev_node = node;
  state.next_node = node->neighbors[2];

  state.front_vertices.resize(1);
  state.front_vertices[0] = *node->upper_opp_edge.end_vertex_it;
  state.front_on_lower_boundary = true;

  triangulate_monotone_channel(state);
}

void triangulate_monotone_channel(TriangulateState& state)
{
  while (true)
  {
    if (state.lower_vertex_it->x() <= state.upper_vertex_it->x() &&
        state.lower_vertex_it->x() <= state.next_node->vertex_it->x())
    {
      if (state.lower_vertex_it == state.next_node->vertex_it)
      {
        if (state.next_node->type == NodeType::leaf)
        {
          DIDA_DEBUG_ASSERT(state.next_node->direction == HorizontalDirection::right);
          triangulate_handle_last_vertex(state, *state.next_node->vertex_it);
          return;
        }
        else
        {
          DIDA_DEBUG_ASSERT(state.next_node->direction == HorizontalDirection::left);
          if (!triangulate_handle_left_branch(state))
          {
            return;
          }
        }
      }
      else
      {
        triangulate_handle_lower_vertex(state, *state.lower_vertex_it);
        state.lower_vertex_it = next_cyclic(state.vertices, state.lower_vertex_it);
      }
    }
    else if (lex_less_than(*state.upper_vertex_it, *state.next_node->vertex_it))
    {
      triangulate_handle_upper_vertex(state, *state.upper_vertex_it);
      state.upper_vertex_it = prev_cyclic(state.vertices, state.upper_vertex_it);
    }
    else
    {
      DIDA_DEBUG_ASSERT(state.next_node->type != NodeType::leaf);

      if (state.next_node->direction == HorizontalDirection::left)
      {
        if (!triangulate_handle_left_branch(state))
        {
          return;
        }
      }
      else
      {
        triangulate_handle_right_branch(state);
      }
    }
  }
}

bool triangulate_handle_left_branch(TriangulateState& state)
{
  uint8_t incoming_branch_index;
  if (state.prev_node == state.next_node->neighbors[1])
  {
    triangulate_handle_upper_vertex(state, *state.next_node->vertex_it);
    state.upper_vertex_it = state.next_node->upper_opp_edge.start_vertex_it;
    incoming_branch_index = 1;
  }
  else
  {
    DIDA_DEBUG_ASSERT(state.prev_node == state.next_node->neighbors[2]);

    triangulate_handle_lower_vertex(state, *state.next_node->vertex_it);
    state.lower_vertex_it = state.next_node->lower_opp_edge.end_vertex_it;
    incoming_branch_index = 2;
  }

  state.prev_node = state.next_node;
  state.next_node = state.next_node->neighbors[0];

  if (state.next_node->vertex_it->x() < state.lower_vertex_it->x() &&
      state.next_node->vertex_it->x() <= state.upper_vertex_it->x() && state.next_node->type == NodeType::branch &&
      state.next_node->direction == HorizontalDirection::right)
  {
    // There's an edge between the vertex of prev_node and next_node, so we should continue with the same branch of
    // next_node as the one we came from in prev_node.

    if (incoming_branch_index == 1)
    {
      state.upper_vertex_it = state.next_node->vertex_it;
    }
    else
    {
      state.lower_vertex_it = state.next_node->vertex_it;
    }

    state.prev_node = state.next_node;
    state.next_node = state.next_node->neighbors[incoming_branch_index];
  }
  else if (state.lower_vertex_it->x() <= state.upper_vertex_it->x())
  {
    if (incoming_branch_index == 1)
    {
      triangulate_handle_last_vertex(state, *state.lower_vertex_it);
      return false;
    }
  }
  else
  {
    if (incoming_branch_index == 2)
    {
      triangulate_handle_last_vertex(state, *state.upper_vertex_it);
      return false;
    }
  }

  return true;
}

void triangulate_handle_right_branch(TriangulateState& state)
{
  if (state.front_on_lower_boundary)
  {
    state.lower_vertex_it = state.next_node->vertex_it;

    state.prev_node = state.next_node;
    state.next_node = state.next_node->neighbors[2];
  }
  else
  {
    state.upper_vertex_it = state.next_node->vertex_it;

    state.prev_node = state.next_node;
    state.next_node = state.next_node->neighbors[1];
  }
}

void triangulate_handle_lower_vertex(TriangulateState& state, Point2 vertex)
{
  if (state.front_on_lower_boundary)
  {
    while (state.front_vertices.size() >= 2 &&
           is_counter_clockwise(*(state.front_vertices.end() - 2), *(state.front_vertices.end() - 1), vertex))
    {
      std::array<Point2, 3> triangle_vertices{
          *(state.front_vertices.end() - 2),
          *(state.front_vertices.end() - 1),
          vertex,
      };
      *(state.result_it++) = Triangle2::unsafe_from_vertices(triangle_vertices);

      state.front_vertices.pop_back();
    }

    state.front_vertices.push_back(vertex);
  }
  else
  {
    for (size_t i = 1; i < state.front_vertices.size(); i++)
    {
      std::array<Point2, 3> triangle_vertices{
          state.front_vertices[i],
          state.front_vertices[i - 1],
          vertex,
      };
      (*state.result_it++) = Triangle2::unsafe_from_vertices(triangle_vertices);
    }

    state.front_vertices[0] = state.front_vertices.back();

    state.front_vertices.resize(2);
    state.front_vertices[1] = vertex;

    state.front_on_lower_boundary = true;
  }
}

void triangulate_handle_upper_vertex(TriangulateState& state, Point2 vertex)
{
  if (!state.front_on_lower_boundary)
  {
    while (state.front_vertices.size() >= 2 &&
           is_counter_clockwise(*(state.front_vertices.end() - 1), *(state.front_vertices.end() - 2), vertex))
    {
      std::array<Point2, 3> triangle_vertices{
          *(state.front_vertices.end() - 1),
          *(state.front_vertices.end() - 2),
          vertex,
      };
      *(state.result_it++) = Triangle2::unsafe_from_vertices(triangle_vertices);
      state.front_vertices.pop_back();
    }

    state.front_vertices.push_back(vertex);
  }
  else
  {
    for (size_t i = 1; i < state.front_vertices.size(); i++)
    {
      std::array<Point2, 3> triangle_vertices{
          state.front_vertices[i - 1],
          state.front_vertices[i],
          vertex,
      };
      *(state.result_it++) = Triangle2::unsafe_from_vertices(triangle_vertices);
    }

    state.front_vertices[0] = state.front_vertices.back();

    state.front_vertices.resize(2);
    state.front_vertices[1] = vertex;

    state.front_on_lower_boundary = false;
  }
}

void triangulate_handle_last_vertex(TriangulateState& state, Point2 vertex)
{
  if (state.front_on_lower_boundary)
  {
    for (size_t i = 1; i < state.front_vertices.size(); i++)
    {
      std::array<Point2, 3> triangle_vertices{
          state.front_vertices[i - 1],
          state.front_vertices[i],
          vertex,
      };

      *(state.result_it++) = Triangle2::unsafe_from_vertices(triangle_vertices);
    }
  }
  else
  {
    for (size_t i = 1; i < state.front_vertices.size(); i++)
    {
      std::array<Point2, 3> triangle_vertices{
          state.front_vertices[i],
          state.front_vertices[i - 1],
          vertex,
      };
      *(state.result_it++) = Triangle2::unsafe_from_vertices(triangle_vertices);
    }
  }
}

bool is_counter_clockwise(Point2 v0, Point2 v1, Point2 v2)
{
  return cross(v1 - v0, v2 - v0) > 0;
}

} // namespace

std::vector<Triangle2> triangulate(VerticesView vertices, const Node* root_node)
{
  DIDA_ASSERT(root_node->type == NodeType::leaf);

  std::vector<Triangle2> result(vertices.size() - 2);
  TriangulateState state{vertices, result.begin()};

  DIDA_ASSERT(root_node->type == NodeType::leaf);

  const Node* node = root_node;
  const Node* prev = root_node->neighbors[0];

  // This loop iterates over the nodes of the vertical decomposition, in the order they'd be encountered by a point
  // traversing the boundary of the polygon in the counter clockwise direction (so this means that all non-leaf nodes
  // are encountered twice).
  //
  // During this traversal, if the current location is a location where a new monotone channel starts, then the relevant
  // function to triangulate the channel is called.
  do
  {
    if (node->type == NodeType::leaf)
    {
      if (node->direction == HorizontalDirection::left)
      {
        triangulate_monotone_channel_from_left_leaf(state, node);
      }

      std::swap(node, prev);
    }
    else
    {
      const Node* next;
      if (node->direction == HorizontalDirection::right)
      {
        if (node->neighbors[0] == prev)
        {
          Point2 lower_left_vertex = *node->lower_opp_edge.start_vertex_it;
          Point2 upper_left_vertex = *node->upper_opp_edge.end_vertex_it;
          if (lower_left_vertex.x() > upper_left_vertex.x() && !lex_less_than(lower_left_vertex, *prev->vertex_it))
          {
            triangulate_monotone_channel_from_lower_right_branch(state, node);
          }

          next = node->neighbors[1];
        }
        else if (node->neighbors[1] == prev)
        {
          next = node->neighbors[2];
        }
        else
        {
          next = node->neighbors[0];

          Point2 lower_left_vertex = *node->lower_opp_edge.start_vertex_it;
          Point2 upper_left_vertex = *node->upper_opp_edge.end_vertex_it;
          if (upper_left_vertex.x() >= lower_left_vertex.x() && !lex_less_than(upper_left_vertex, *next->vertex_it))
          {
            triangulate_monotone_channel_from_upper_right_branch(state, node);
          }
        }
      }
      else
      {
        next = node->neighbors[0] == prev ? node->neighbors[2]
                                          : (node->neighbors[1] == prev ? node->neighbors[0] : node->neighbors[1]);
      }

      prev = node;
      node = next;
    }
  } while (node != root_node);

  DIDA_ASSERT(state.result_it == result.end());

  return result;
}

} // namespace dida::detail::vertical_decomposition