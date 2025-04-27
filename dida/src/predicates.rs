use crate::Point2;
use crate::convex::convex_polygon::ConvexPolygon;
use crate::convex::intersect::intersect;

pub trait Intersects<Rhs> {
    /// Returns whether two geometric objects intersect.
    fn intersects(&self, b: &Rhs) -> bool;
}

impl<AVertexContainer, BVertexContainer>Intersects<ConvexPolygon<BVertexContainer>>
    for ConvexPolygon<AVertexContainer> where
        AVertexContainer: AsRef<[Point2]>,
        BVertexContainer: AsRef<[Point2]>,
{
    fn intersects(&self, b: &ConvexPolygon<BVertexContainer>) -> bool {
        intersect(self.as_view(), b.as_view())
    }
}
