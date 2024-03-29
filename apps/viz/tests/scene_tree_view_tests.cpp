#include "../scene_tree_view.hpp"

#include <catch2/catch_test_macros.hpp>

namespace dida::viz
{

namespace
{

struct TestScene
{
  TestScene()
  {
    scene = std::make_shared<VizScene>();

    polygon_foo = std::make_shared<VizPolygon>(
        "foo", std::vector<Point2>{{3.38, 4.62}, {4.78, 2.6}, {6.78, 2.08}, {4.54, 1.82}, {2.86, 0.08}, {3.98, 2.48}},
        true);
    scene->add_primitive(polygon_foo);

    polygon_bar = std::make_shared<VizPolygon>(
        "bar", std::vector<Point2>{{1.46, 3.88}, {2.78, 2.4}, {4.96, 4.22}, {2.84, 0.94}}, true);
    scene->add_primitive(polygon_bar);
  }

  std::shared_ptr<VizScene> scene;

  std::shared_ptr<VizPolygon> polygon_foo;

  std::shared_ptr<VizPolygon> polygon_bar;
};

} // namespace

TEST_CASE("SceneTreeModel::data")
{
  TestScene test_scene;
  SceneTreeModel tree_model(test_scene.scene);

  SECTION("Primitive")
  {
    QModelIndex foo_index = tree_model.index(0, 0, QModelIndex());
    QVariant foo_text = tree_model.data(foo_index, Qt::DisplayRole);
    CHECK(foo_text == "foo");

    QModelIndex bar_index = tree_model.index(1, 0, QModelIndex());
    QVariant bar_text = tree_model.data(bar_index, Qt::DisplayRole);
    CHECK(bar_text == "bar");
  }

  SECTION("Vertex")
  {
    QModelIndex foo_index = tree_model.index(0, 0, QModelIndex());
    QModelIndex foo_vertex_index = tree_model.index(1, 0, foo_index);
    QVariant foo_vertex_text = tree_model.data(foo_vertex_index, Qt::DisplayRole);
    CHECK(foo_vertex_text == "1: {4.78, 2.60}");

    QModelIndex bar_index = tree_model.index(1, 0, QModelIndex());
    QModelIndex bar_vertex_index = tree_model.index(2, 0, bar_index);
    QVariant bar_vertex_text = tree_model.data(bar_vertex_index, Qt::DisplayRole);
    CHECK(bar_vertex_text == "2: {4.96, 4.22}");
  }
}

TEST_CASE("SceneTreeModel::parent")
{
  TestScene test_scene;
  SceneTreeModel tree_model(test_scene.scene);

  SECTION("Parent of primitive")
  {
    QModelIndex primitive_index = tree_model.index(1, 0, QModelIndex());
    CHECK(tree_model.parent(primitive_index) == QModelIndex());
  }

  SECTION("Parent of vertex")
  {
    QModelIndex primitive_index = tree_model.index(1, 0, QModelIndex());
    QModelIndex vertex_index = tree_model.index(2, 0, primitive_index);
    CHECK(tree_model.parent(vertex_index) == primitive_index);
  }
}

TEST_CASE("SceneTreeModel::rowCount")
{
  TestScene test_scene;
  SceneTreeModel tree_model(test_scene.scene);

  SECTION("Root")
  {
    CHECK(tree_model.rowCount(QModelIndex()) == 2);
  }

  SECTION("Primitive")
  {
    QModelIndex primitive_index = tree_model.index(0, 0, QModelIndex());
    CHECK(tree_model.rowCount(primitive_index) == 6);
  }

  SECTION("Vertex")
  {
    QModelIndex primitive_index = tree_model.index(1, 0, QModelIndex());
    QModelIndex vertex_index = tree_model.index(2, 0, primitive_index);
    CHECK(tree_model.rowCount(vertex_index) == 0);
  }
}

TEST_CASE("SceneTreeModel::columnCount")
{
  TestScene test_scene;
  SceneTreeModel tree_model(test_scene.scene);

  QModelIndex primitive_index = tree_model.index(0, 0, QModelIndex());
  CHECK(tree_model.columnCount(primitive_index) == 1);
}

TEST_CASE("SceneTreeModel emits beginInsertRows and endInsertRows")
{
  std::shared_ptr<VizScene> scene = std::make_shared<VizScene>();
  SceneTreeModel tree_model(scene);

  size_t about_to_be_inserted_index = 0;
  size_t inserted_index = 0;

  QObject::connect(&tree_model, &SceneTreeModel::rowsAboutToBeInserted,
                   [&about_to_be_inserted_index, &scene](QModelIndex parent, int first, int last)
                   {
                     CHECK(scene->primitives().size() == about_to_be_inserted_index);

                     CHECK(!parent.isValid());
                     CHECK(first == about_to_be_inserted_index);
                     CHECK(last == about_to_be_inserted_index);

                     about_to_be_inserted_index++;
                   });

  QObject::connect(&tree_model, &SceneTreeModel::rowsInserted,
                   [&inserted_index, &scene](QModelIndex parent, int first, int last)
                   {
                     REQUIRE(scene->primitives().size() == inserted_index + 1);

                     CHECK(!parent.isValid());
                     CHECK(first == inserted_index);
                     CHECK(last == inserted_index);

                     inserted_index++;
                   });

  scene->add_primitive(std::make_shared<VizPolygon>(
      "foo", std::vector<Point2>{{3.38, 4.62}, {4.78, 2.6}, {6.78, 2.08}, {4.54, 1.82}, {2.86, 0.08}, {3.98, 2.48}},
      true));
  scene->add_primitive(std::make_shared<VizPolygon>(
      "bar", std::vector<Point2>{{1.46, 3.88}, {2.78, 2.4}, {4.96, 4.22}, {2.84, 0.94}}, true));

  CHECK(about_to_be_inserted_index == 2);
  CHECK(inserted_index == 2);
}

} // namespace dida::viz