use crate::Point2;
use crate::convex::validation::are_valid_convex_polygon_vertices;
use crate::cyclic::unimodal_cyclic_sequence_maximum;
use std::ops::Index;
use std::str::FromStr;

pub struct ConvexPolygon<VertexContainer: AsRef<[Point2]>> {
    vertices: VertexContainer,
}

pub type ConvexPolygonView<'a> = ConvexPolygon<&'a [Point2]>;

impl<VertexContainer: AsRef<[Point2]>> ConvexPolygon<VertexContainer> {
    /// Constructs a new `ConvexPolygonView` with the given vertices.
    ///
    /// This function will panic if the vertices do not form a valid convex polyogn (see
    /// 'are_valid_convex_polygon_vertices').
    pub fn new(vertices: VertexContainer) -> Self {
        std::assert!(are_valid_convex_polygon_vertices(vertices.as_ref()));
        Self { vertices }
    }

    /// An unchecked variant of `ConvexPolygonView::new`.
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
    pub fn as_view(&self) -> ConvexPolygonView {
        ConvexPolygonView {
            vertices: self.vertices.as_ref(),
        }
    }

    /// Returns the index of the leftmost vertex (the vertex which compares less than all other vertices with
    /// `Point2::lex_less_than`).
    pub fn leftmost_vertex_index(&self) -> usize {
        unimodal_cyclic_sequence_maximum(self.vertices(), |a, b| a.x() < b.x())
    }
    
    /// Returns the index of the rightmost vertex (the vertex which compares greater than all other vertices with
    /// `Point2::lex_greater_than`).
    pub fn rightmost_vertex_index(&self) -> usize {
        unimodal_cyclic_sequence_maximum(self.vertices(), |a, b| a.x() > b.x())
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
}
