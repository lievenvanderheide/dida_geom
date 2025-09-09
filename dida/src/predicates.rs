use crate::Point2;
use crate::convex::convex_polygon::ConvexPolygon;

pub trait Intersects<Rhs> {
    /// Returns whether two geometric objects intersect.
    fn intersects(&self, b: &Rhs) -> bool;
}

impl<AVertexContainer, BVertexContainer> Intersects<ConvexPolygon<BVertexContainer>>
    for ConvexPolygon<AVertexContainer> where
        AVertexContainer: AsRef<[Point2]>,
        BVertexContainer: AsRef<[Point2]>,
{
    fn intersects(&self, b: &ConvexPolygon<BVertexContainer>) -> bool {
        crate::convex::intersect::intersect(self.as_view(), b.as_view())
    }
}

pub trait Distance<Rhs> {
    fn distance_squared(&self, b: &Rhs) -> f64 {
        let distance = self.distance(b);
        distance * distance
    }

    fn distance(&self, b: &Rhs) -> f64 {
        self.distance_squared(b).sqrt()
    }
}

impl<AVertexContainer, BVertexContainer> Distance<ConvexPolygon<BVertexContainer>>
    for ConvexPolygon<AVertexContainer> where
        AVertexContainer: AsRef<[Point2]>,
        BVertexContainer: AsRef<[Point2]>,
{
    fn distance_squared(&self, b: &ConvexPolygon<BVertexContainer>) -> f64 {
        crate::convex::distance::distance_squared(self.as_view(), b.as_view())
    }
}