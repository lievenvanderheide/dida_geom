use crate::{Point2, Vec2, ScalarDeg1};
use crate::convex::validation::are_valid_convex_polygon_vertices;
use crate::convex::convex_hull::convex_hull_inplace;
use crate::cyclic::unimodal_cyclic_sequence_maximum;
use std::ops::Index;
use std::str::FromStr;

#[derive(Debug, PartialEq)]
pub struct ConvexPolygon<VertexContainer: AsRef<[Point2]>> {
    vertices: VertexContainer,
}

pub type ConvexPolygonView<'a> = ConvexPolygon<&'a [Point2]>;

impl<VertexContainer: AsRef<[Point2]>> ConvexPolygon<VertexContainer> {
    /// Constructs a new `ConvexPolygon` with the given vertices.
    ///
    /// This function will panic if the vertices do not form a valid convex polyogn (see
    /// 'are_valid_convex_polygon_vertices').
    pub fn new(vertices: VertexContainer) -> Self {
        std::assert!(are_valid_convex_polygon_vertices(vertices.as_ref()));
        Self { vertices }
    }

    /// An unchecked variant of `ConvexPolygon::new`.
    pub fn new_unchecked(vertices: VertexContainer) -> Self {
        std::debug_assert!(are_valid_convex_polygon_vertices(vertices.as_ref()));
        Self { vertices }
    }

    /// Returns the number of vertices of this polygon.
    pub fn num_vertices(&self) -> usize {
        self.vertices.as_ref().len()
    }

    /// Returns a slice of the vertices.
    pub fn vertices(&self) -> &[Point2] {
        self.vertices.as_ref()
    }

    /// Returns a ConvexPolygonView of this polygon.
    pub fn as_view(&self) -> ConvexPolygonView<'_> {
        ConvexPolygonView {
            vertices: self.vertices.as_ref(),
        }
    }

    /// Returns the index of the leftmost vertex (the vertex which compares less than all other vertices with
    /// `Point2::lex_less_than`).
    pub fn leftmost_vertex_index(&self) -> usize {
        unimodal_cyclic_sequence_maximum(self.vertices(), |a, b| a.x() > b.x())
    }
    
    /// Returns the index of the rightmost vertex (the vertex which compares greater than all other vertices with
    /// `Point2::lex_greater_than`).
    pub fn rightmost_vertex_index(&self) -> usize {
        unimodal_cyclic_sequence_maximum(self.vertices(), |a, b| a.x() < b.x())
    }
}

impl<VertexContainer: AsMut<[Point2]> + AsRef<[Point2]>> ConvexPolygon<VertexContainer> {
    pub fn rotate_vertex_list_right(&mut self, k: usize) {
        self.vertices.as_mut().rotate_right(k);
    }
}

impl<VertexContainer: Clone + AsRef<[Point2]>> Clone for ConvexPolygon<VertexContainer> {
    fn clone(&self) -> Self {
        ConvexPolygon {
            vertices: self.vertices.clone(),
        }
    }
}

impl ConvexPolygon<Vec<Point2>> {
    /// Returns the convex hull of the given point set as a `ConvexPolygon<Vec<Point2>>`.
    ///
    /// To avoid invalid convex polygons when the convex hull degenerates into a point or a line segment, a very small
    /// or very thin polygon with non-zero area which closely resembles the point or line segment is returned instead.
    pub fn convex_hull(points: &[Point2]) -> Self {
        let mut vertices = points.to_vec();
        let num_vertices = convex_hull_inplace(&mut vertices);
        vertices.truncate(num_vertices);

        if num_vertices == 2 {
            if Point2::lex_greater_than(vertices[0], vertices[1]) {
                vertices.swap(0, 1);
            }

            let offset = if vertices[0].x() != vertices[1].x() {
                Vec2::new(0.0, ScalarDeg1::QUANTUM)
            } else {
                Vec2::new(-ScalarDeg1::QUANTUM, 0.0)
            };

            vertices.push(vertices[0] + offset);
        } else if num_vertices == 1 {
            vertices.extend_from_slice(&[
                vertices[0] + Vec2::new(ScalarDeg1::QUANTUM, 0.0),
                vertices[0] + Vec2::new(0.0, ScalarDeg1::QUANTUM)
            ]);
        }

        ConvexPolygon::new_unchecked(vertices)
    }
}

impl<'a> Copy for ConvexPolygonView<'a> {}

impl<'a> Index<usize> for ConvexPolygonView<'a> {
    type Output = Point2;

    fn index(&self, index: usize) -> &Self::Output {
        &self.vertices[index]
    }
}

impl FromStr for ConvexPolygon<Vec<Point2>> {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, String> {
        let vertices = Point2::vec_from_str(s)?;

        if !are_valid_convex_polygon_vertices(&vertices) {
            return Err(String::from("Vertices don't form a valid convex polygon."));
        }

        Ok(ConvexPolygon::new(vertices))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new_and_access() {
        let vertices = Point2::vec_from_str("{{2.84, -3.1}, {8.68, 2.44}, {3.34, 4.38}, {-0.8, 1.52}}").unwrap();
        let polygon = ConvexPolygonView::new(vertices.as_ref());
        std::assert_eq!(polygon.num_vertices(), 4);
        std::assert_eq!(polygon[0], Point2::new(2.84, -3.1));
        std::assert_eq!(polygon[1], Point2::new(8.68, 2.44));
        std::assert_eq!(polygon[2], Point2::new(3.34, 4.38));
        std::assert_eq!(polygon[3], Point2::new(-0.8, 1.52));
    }

    #[test]
    fn test_new_unchecked_and_access() {
        let vertices = Point2::vec_from_str("{{2.62, 2.44}, {8.52, 3.6}, {7.38, 4.58}}").unwrap();
        let polygon = ConvexPolygonView::new_unchecked(vertices.as_ref());
        std::assert_eq!(polygon.num_vertices(), 3);
        std::assert_eq!(polygon[0], Point2::new(2.62, 2.44));
        std::assert_eq!(polygon[1], Point2::new(8.52, 3.6));
        std::assert_eq!(polygon[2], Point2::new(7.38, 4.58));
    }

    #[test]
    fn test_convex_hull() {
        // General case.
        std::assert_eq!(
            ConvexPolygon::convex_hull(
                &Point2::vec_from_str(
                    "{{-6, 6}, {3, -2}, {16, 5}, {10, -6}, {-3, -6}, {-6, 1}, {2, 4}, {13, -4}}"
                ).unwrap()
            ),
            ConvexPolygon::new(
                Point2::vec_from_str("{{-6, 1}, {-3, -6}, {10, -6}, {13, -4}, {16, 5}, {-6, 6}}").unwrap()
            )
        );

        // Degenerates into a point.
        std::assert_eq!(
            ConvexPolygon::convex_hull(&Point2::vec_from_str("{{0.32, 5.47}, {0.32, 5.47}}").unwrap()),
            ConvexPolygon::new(vec![
                Point2::new(0.32, 5.47),
                Point2::new(0.32 + ScalarDeg1::QUANTUM, 5.47),
                Point2::new(0.32, 5.47 + ScalarDeg1::QUANTUM),
            ])
        );

        // Degenerates into a non-vertical line segment.
        std::assert_eq!(
            ConvexPolygon::convex_hull(
                &Point2::vec_from_str("{{-6, 4}, {0, 6}, {-3, 5}, {6, 8}}").unwrap()
            ),
            ConvexPolygon::new(vec![
              Point2::new(-6.0, 4.0),
              Point2::new(6.0, 8.0),
              Point2::new(-6.0, 4.0 + ScalarDeg1::QUANTUM),
            ])
        );

        // Degenerates into a vertical line segment.
        std::assert_eq!(
            ConvexPolygon::convex_hull(
                &Point2::vec_from_str("{{5, -4}, {5, 5}, {5, 1}, {5, 11}}").unwrap()
            ),
            ConvexPolygon::new(vec![
              Point2::new(5.0, -4.0),
              Point2::new(5.0, 11.0),
              Point2::new(5.0 - ScalarDeg1::QUANTUM, -4.0),
            ])
        );
    }
}
