use crate::Point2;
use crate::convex::validation::are_valid_convex_polygon_vertices;
use std::ops::Index;    

pub struct ConvexPolygonView<'a> {
    vertices: &'a [Point2]
}

impl<'a> ConvexPolygonView<'a> {
    pub fn new(vertices: &'a [Point2]) -> Self {
        std::assert!(are_valid_convex_polygon_vertices(vertices.as_ref()));
        Self { vertices }
    }

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
}