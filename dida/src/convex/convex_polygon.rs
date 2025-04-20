use crate::Point2;
use crate::convex::validation::are_valid_convex_polygon_vertices;
use std::ops::Index;    

pub struct ConvexPolygonView<'a> {
    vertices: &'a [Point2]
}

impl<'a> ConvexPolygonView<'a> {
    /// Constructs a new 'ConvexPolygonView' with the given vertices.
    ///
    /// This function will panic if the vertices do not form a valid convex polyogn (see
    /// 'are_valid_convex_polygon_vertices').
    pub fn new(vertices: &'a [Point2]) -> Self {
        std::assert!(are_valid_convex_polygon_vertices(vertices.as_ref()));
        Self { vertices }
    }

    /// An unchecked variant of 'ConvexPolygonView::new'.
    pub fn new_unchecked(vertices: &'a [Point2]) -> Self {
        std::debug_assert!(are_valid_convex_polygon_vertices(vertices.as_ref()));
        Self { vertices }
    }

    /// Returns the number of vertices of this polygon.
    pub fn num_vertices(&self) -> usize {
        self.vertices.len()
    }
}

impl<'a> Index<usize> for ConvexPolygonView<'a> {
    type Output = Point2;

    fn index(&self, index: usize) -> &Self::Output {
        &self.vertices[index]
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
