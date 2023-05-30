
#pragma once

#include <QtCore/QObject>
#include <string>
#include <vector>

#include "dida/array_view.hpp"
#include "dida/point2.hpp"

namespace dida
{

class Parser;

}

namespace dida::viz
{

/// A polygon in a @c VizScene.
class VizPolygon : public QObject
{
  Q_OBJECT

public:
  /// Constructs a @c VizPolygon with the given values.
  ///
  /// @param name The name of the polygon. See @c name().
  /// @param vertices The vertices of the polygon. See @c vertices().
  /// @param should_be_convex Whether the polygon should be convex. See @c should_be_convex().
  inline VizPolygon(std::string name, std::vector<Point2> vertices, bool should_be_convex);

  /// Returns the name of this @c VizPolygon.
  ///
  /// @return The name.
  inline const std::string& name() const;

  /// Returns the vertices of this @c VizPolygon.
  ///
  /// @return The vertices.
  inline const std::vector<Point2>& vertices() const;

  /// Adds a vertex to this @c VizPolygon.
  ///
  /// The @c will_add_vertex signal is emitted before the change is made, the @c vertex_added and @c data_changed
  /// signals are emitted after the change is made
  ///
  /// @param add_vertex The vertex to add.
  void add_vertex(Point2 vertex);

  /// Returns whether this @c VizPolygon should be convex.
  ///
  /// This flag affects the result of the @c is_valid function, but convexity is not otherwise enforced.
  inline bool should_be_convex() const;

  /// Returns whether the vertices of this @c VizPolygon form a valid @c Polygon2 or @c ConvexPolygon2 (depending on
  /// whether @c should_be_convex() is true).
  ///
  /// @return True iff the vertices form a valid polygon of the expected type.
  inline bool is_polygon_valid() const;

Q_SIGNALS:
  /// This signal is emitted when a new vertex is about to be added to this @c VizPolygon.
  ///
  /// @param index The index the new vertex will have.
  void will_add_vertex(size_t index);

  /// This signal is emitted after a new vertex is added to this @c VizPolygon.
  ///
  /// @param index The index of the newly added vertex.
  void vertex_added(size_t index);

  /// This signal is emitted when this polygon changes.
  void data_changed();

private:
  void update_is_polygon_valid();

  std::string name_;
  std::vector<Point2> vertices_;
  bool should_be_convex_;
  bool is_polygon_valid_;
};

/// Parses a @c VizPolygon using the given parser.
///
/// On success, the parser's head will have advanced to the first character after the matched characters, on failure,
/// the parses is left in an undefined state.
///
/// @param parser The parser.
/// @return A pointer to the @c VizPolygon, or @c nullptr on failure.
std::shared_ptr<VizPolygon> parse_viz_polygon(Parser& parser);

/// Parses a @c VizPolygon from the given string.
///
/// @param string The string to parse.
/// @return A pointer to the @c VizPolygon, or @c nullptr on failure.
std::shared_ptr<VizPolygon> parse_viz_polygon(std::string_view string);

std::ostream& operator<<(std::ostream& s, const VizPolygon& polygon);

/// The scene containing all the primitives currently shown in the visualizer.
class VizScene : public QObject
{
  Q_OBJECT

public:
  /// Constructs an empty @c VizScene.
  VizScene() = default;

  /// Constructs a @c VizScene with the given primities.
  ///
  /// @param primitives The primitives.
  inline VizScene(std::vector<std::shared_ptr<VizPolygon>> primitives);

  /// Returns the primitives of this @c VizScene.
  ///
  /// @return The primitives.
  inline const std::vector<std::shared_ptr<VizPolygon>>& primitives();

  /// Adds a primitive to this @c VizScene.
  ///
  /// This function will emit the @c will_add_primitive signal before any changes are made to the @c VizScene, and
  /// the @c primitive_added and @c data_changed signals after the changes have been made.
  ///
  /// @param polygon The polygon to add.
  void add_primitive(std::shared_ptr<VizPolygon> polygon);

Q_SIGNALS:
  /// This signal is emitted whenever some data in this scene changes.
  void data_changed();

  /// This signal is emitted right before a new primitive is added.
  ///
  /// @param index The index of the new primitive.
  void will_add_primitive(size_t index);

  /// This signal is emitted after a new primitive was added.
  ///
  /// @param index The index of the new primitive.
  void primitive_added(size_t index);

private:
  std::vector<std::shared_ptr<VizPolygon>> primitives_;
};

} // namespace dida::viz

#include "dida/viz/scene.inl"