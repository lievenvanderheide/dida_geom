#include "dida/polygon2_utils.hpp"

#include "dida/detail/vertical_decomposition/divide_and_conquer_builder.hpp"
#include "dida/detail/vertical_decomposition/triangulate.hpp"

namespace dida
{

bool geometrically_equal(PolygonView2 a, PolygonView2 b)
{
  PolygonView2::const_iterator b_it = std::find(b.begin(), b.end(), a[0]);
  if (b_it == b.end())
  {
    return false;
  }

  b_it = next_cyclic(b, b_it);

  for (PolygonView2::const_iterator a_it = a.begin() + 1; a_it != a.end(); ++a_it)
  {
    if (*a_it != *b_it)
    {
      return false;
    }

    b_it = next_cyclic(b, b_it);
  }

  return true;
}

std::vector<Triangle2> triangulate(PolygonView2 polygon)
{
  using namespace detail::vertical_decomposition;

  NodePool node_pool;
  Node* root_node = vertical_decomposition_with_divide_and_conquer_builder(
      polygon, node_pool, VerticalDecompositionType::interior_decomposition);
  return triangulate(polygon, root_node);
}

} // namespace dida