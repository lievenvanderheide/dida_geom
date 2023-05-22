#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

YOnEdge y_on_edge_for_x(Segment2 edge, ScalarDeg1 x)
{
  DIDA_DEBUG_ASSERT(edge.start().x() != edge.end().x());

  // start_x + t * dir_x = x
  // t = (x - start_x) / dir_x
  // y = start_y + t * dir_y
  // y = start_y + (x - start_x) / dir_x * dir_y
  // y = (start_y * dir_x + (x - start_x) * dir_y) / dir_x

  Point2 start = edge.start();
  Vector2 dir = edge.end() - edge.start();

  ScalarDeg2 num = start.y() * dir.x() + (x - start.x()) * dir.y();
  ScalarDeg1 denom = dir.x();
  if (denom < 0)
  {
    num = -num;
    denom = -denom;
  }

  return YOnEdge(num, denom);
}

VerticalDecompositionRegionsIterator::VerticalDecompositionRegionsIterator(const Node* first_node)
{
  first_node_ = first_node;
  cur_node_ = first_node;
  direction_ = first_node->direction;

  uint8_t branch_index = direction_ == HorizontalDirection::left ? 1 : 2;
  next_node_ = first_node->neighbors[branch_index];

  // leaf_region_branch_index_ is only defined when next_node_ == nullptr, but there's no harm in always setting
  // it.
  leaf_region_branch_index_ = branch_index;

  if (should_skip_current_region())
  {
    move_next();
  }
}

bool VerticalDecompositionRegionsIterator::move_next()
{
  if (!next_node_)
  {
    // We're at a leaf region.

    next_node_ = cur_node_;
    cur_node_ = nullptr;
    direction_ = direction_ == HorizontalDirection::left ? HorizontalDirection::right : HorizontalDirection::left;
  }

  do
  {
    if (next_node_->direction == direction_)
    {
      // The direction of 'next_node_' is equal to the current direction.

      cur_node_ = next_node_;

      uint8_t branch_index = direction_ == HorizontalDirection::right ? 1 : 2;
      next_node_ = next_node_->neighbors[branch_index];

      // leaf_region_branch_index_ is only defined when next_node_ == nullptr, but there's no harm in always setting
      // it.
      leaf_region_branch_index_ = branch_index;
    }
    else
    {
      // The direction of 'next_node_' is opposite to the current direction.

      // If cur_node_ == nullptr, then we're exiting a leaf, so the branch index is 'leaf_region_branch_index_',
      // otherwise the incoming branch index is determined by comparing the pointers in 'next_node_->neighbors' to
      // 'cur_node_'.
      uint8_t incoming_branch_index =
          cur_node_ ? (next_node_->neighbors[1] == cur_node_ ? 1 : 2) : leaf_region_branch_index_;

      if (incoming_branch_index == (direction_ == HorizontalDirection::left ? 1 : 2))
      {
        // We're turning around, entering the branch above/below the incoming branch.

        if (next_node_ == first_node_)
        {
          return false;
        }

        cur_node_ = next_node_;

        uint8_t outgoing_brach_index = direction_ == HorizontalDirection::left ? 2 : 1;
        next_node_ = next_node_->neighbors[outgoing_brach_index];
        direction_ = direction_ == HorizontalDirection::left ? HorizontalDirection::right : HorizontalDirection::left;

        // leaf_region_branch_index_ is only defined when next_node_ == nullptr, but there's no harm in always setting
        // it.
        leaf_region_branch_index_ = outgoing_brach_index;
      }
      else
      {
        // We're continuing in the same direction, entering the region between next_node_->lower_opp_edge and
        // node_->upper_opp_edge.

        cur_node_ = next_node_;
        next_node_ = next_node_->neighbors[0];

        // leaf_region_branch_index_ is only defined when next_node_ == nullptr, but there's no harm in always setting
        // it.
        leaf_region_branch_index_ = 0;
      }
    }
  } while (should_skip_current_region());

  return true;
}

bool VerticalDecompositionRegionsIterator::should_skip_current_region() const
{
  // We should skip the current region if we're on the upper boundary of a region which also has a lower boundary.

  // If we're going towards the right, then we're on the lower boundary of the current region, so we should not skip it.
  if (direction_ == HorizontalDirection::right)
  {
    return false;
  }

  // If there's no next node, then we're in a leaf region, so we shouldn't skip it.
  if (!next_node_)
  {
    return false;
  }

  // If we get here, then we're on the upper boundary of the current region.

  // If lower_opp_edge is set, then we're sure there's a lower boundary, and since we're currently on the upper
  // boundary, we should skip it.
  if (cur_node_->lower_opp_edge.is_valid())
  {
    return true;
  }

  if (cur_node_->direction == HorizontalDirection::left)
  {
    // We're going to the left, and the direction of the current node is towards the left too. If we're inside the upper
    // branch, then there's a lower boundary, so the current region should be skipped, if we're inside the lower branch,
    // then there's no lower edge, so we shouldn't skip it.
    return next_node_ == cur_node_->neighbors[2];
  }
  else
  {
    // We're going to the left, and the direction of the current node is towards the right, so we're on the side of the
    // region which extends from cur_node->lower_opp_edge to cur_node->upper_opp_edge. Since we already know that
    // cur_node->lower_opp_edge is invalid, we know the current region doesn't have a lower boundary, so we shouldn't
    // skip it.
    return false;
  }
}

} // namespace dida::detail::vertical_decomposition