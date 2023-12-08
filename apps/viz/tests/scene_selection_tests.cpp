#include "../scene_selection.hpp"

#include <catch2/catch_test_macros.hpp>

namespace dida::viz
{

namespace
{

TEST_CASE("VizSceneSelection::select_vertex/deselect_vertex and access")
{
  SECTION("select_vertex and deselect_vertex change is_vertex_selected return value")
  {
    VizSceneSelection selection;

    CHECK_FALSE(selection.is_vertex_selected(1, 2));
    selection.select_vertex(1, 2);
    CHECK(selection.is_vertex_selected(1, 2));

    selection.deselect_vertex(1, 2);
    CHECK_FALSE(selection.is_vertex_selected(1, 2));
  }

  SECTION("Select/deselect already selected/deslected")
  {
    VizSceneSelection selection;

    CHECK_FALSE(selection.is_vertex_selected(1, 2));
    selection.select_vertex(1, 2);
    CHECK(selection.is_vertex_selected(1, 2));
    selection.select_vertex(1, 2);
    CHECK(selection.is_vertex_selected(1, 2));

    selection.deselect_vertex(1, 2);
    CHECK_FALSE(selection.is_vertex_selected(1, 2));
    selection.deselect_vertex(1, 2);
    CHECK_FALSE(selection.is_vertex_selected(1, 2));
  }

  SECTION("Emits selection_changed event")
  {
    bool callback_called = false;

    VizSceneSelection selection;
    QObject::connect(&selection, &VizSceneSelection::selection_changed,
                     [&callback_called]() { callback_called = true; });

    selection.select_vertex(1, 2);
    CHECK(callback_called);

    SECTION("select_vertex shouldn't emit select_changed if already selected")
    {
      callback_called = false;
      selection.select_vertex(1, 2);
      CHECK_FALSE(callback_called);
    }

    callback_called = false;
    selection.deselect_vertex(1, 2);
    CHECK(callback_called);

    SECTION("deselect_vertex shouldn't emit select_changed if not selected")
    {
      callback_called = false;
      selection.deselect_vertex(1, 2);
      CHECK_FALSE(callback_called);
    }
  }
}

} // namespace

} // namespace dida::viz