use crate::Vec2f;
use std::cmp::Ordering;
use std::ops::{Add, AddAssign, Sub, SubAssign};

/// A 2D vector with [`f64`] coordinates.
#[derive(Copy, Clone, PartialEq)]
pub struct Point2f {
    pos: Vec2f,
}

impl Point2f {
    /// Constructs a `Point2` with the given coordinates.
    pub fn new(x: f64, y: f64) -> Self {
        Self {
            pos: Vec2f::new(x, y),
        }
    }

    /// Constructs a `Point2f` from a `Vec2f`.
    pub fn from_vec2f(pos: Vec2f) -> Self {
        Self { pos }
    }

    /// Returns the Vec2f from the origin to this point.
    pub fn to_vec2f(&self) -> Vec2f {
        self.pos
    }

    /// Returns the `x` coordinate of this point.
    pub fn x(self) -> f64 {
        self.pos.x()
    }

    /// Returns the `y` coordinate of this point.
    pub fn y(self) -> f64 {
        self.pos.y()
    }

    /// Sets the `x` coordinate of this point.
    pub fn set_x(&mut self, x: f64) {
        self.pos.set_x(x);
    }

    /// Sets the `y` coordinate of this point.
    pub fn set_y(&mut self, y: f64) {
        self.pos.set_y(y);
    }

    /// Returns whether `a` is lexicographically less than `b`.
    pub fn lex_less_than(a: Point2f, b: Point2f) -> bool {
        Vec2f::lex_less_than(a.pos, b.pos)
    }

    /// Returns whether `a` is lexicographically greater than `b`.
    pub fn lex_greater_than(a: Point2f, b: Point2f) -> bool {
        Vec2f::lex_greater_than(a.pos, b.pos)
    }

    /// Lexicographically compares vectors `a` and `b`.
    pub fn lex_cmp(a: Point2f, b: Point2f) -> Ordering {
        Vec2f::lex_cmp(a.pos, b.pos)
    }
}

impl Add<Vec2f> for Point2f {
    type Output = Self;

    fn add(self, b: Vec2f) -> Self {
        Self { pos: self.pos + b }
    }
}

impl AddAssign<Vec2f> for Point2f {
    fn add_assign(&mut self, b: Vec2f) {
        self.pos += b;
    }
}

impl Sub for Point2f {
    type Output = Vec2f;

    fn sub(self, b: Point2f) -> Vec2f {
        self.pos - b.pos
    }
}

impl Sub<Vec2f> for Point2f {
    type Output = Self;

    fn sub(self, b: Vec2f) -> Self {
        Self { pos: self.pos - b }
    }
}

impl SubAssign<Vec2f> for Point2f {
    fn sub_assign(&mut self, b: Vec2f) {
        self.pos -= b;
    }
}

impl std::fmt::Debug for Point2f {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        self.pos.fmt(f)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new() {
        let point = Point2f::new(8.4, -5.6);
        std::assert_eq!(point.x(), 8.4);
        std::assert_eq!(point.y(), -5.6);
    }

    #[test]
    fn test_from_vec2f() {
        let point = Point2f::from_vec2f(Vec2f::new(6.4, -4.0));
        std::assert_eq!(point.x(), 6.4);
        std::assert_eq!(point.y(), -4.0);
    }

    #[test]
    fn test_to_vec2f() {
        let vec = Point2f::new(9.3, 4.0).to_vec2f();
        std::assert_eq!(vec.x(), 9.3);
        std::assert_eq!(vec.y(), 4.0);
    }

    #[test]
    fn test_set_x() {
        let mut point = Point2f::new(7.1, 8.0);
        point.set_x(-6.0);
        std::assert_eq!(point, Point2f::new(-6.0, 8.0));
    }

    #[test]
    fn test_set_y() {
        let mut point = Point2f::new(8.4, 3.4);
        point.set_y(-4.9);
        std::assert_eq!(point, Point2f::new(8.4, -4.9));
    }

    #[test]
    fn test_lex_less_than() {
        let a = Point2f::new(1.0, 2.0);
        let b = Point2f::new(1.0, 3.0);
        let c = Point2f::new(3.0, 4.0);

        std::assert!(Point2f::lex_less_than(a, b));
        std::assert!(Point2f::lex_less_than(a, c));
        std::assert!(Point2f::lex_less_than(b, c));

        std::assert!(!Point2f::lex_less_than(b, a));
        std::assert!(!Point2f::lex_less_than(c, a));
        std::assert!(!Point2f::lex_less_than(c, b));

        std::assert!(!Point2f::lex_less_than(a, a));
    }

    #[test]
    fn test_lex_greater_than() {
        let a = Point2f::new(1.0, 2.0);
        let b = Point2f::new(1.0, 3.0);
        let c = Point2f::new(3.0, 4.0);

        std::assert!(!Point2f::lex_greater_than(a, b));
        std::assert!(!Point2f::lex_greater_than(a, c));
        std::assert!(!Point2f::lex_greater_than(b, c));

        std::assert!(Point2f::lex_greater_than(b, a));
        std::assert!(Point2f::lex_greater_than(c, a));
        std::assert!(Point2f::lex_greater_than(c, b));

        std::assert!(!Point2f::lex_greater_than(a, a));
    }

    #[test]
    fn test_lex_cmp() {
        let a = Point2f::new(1.0, 2.0);
        let b = Point2f::new(1.0, 3.0);
        let c = Point2f::new(3.0, 4.0);

        std::assert_eq!(Point2f::lex_cmp(a, b), Ordering::Less);
        std::assert_eq!(Point2f::lex_cmp(a, c), Ordering::Less);
        std::assert_eq!(Point2f::lex_cmp(b, c), Ordering::Less);

        std::assert_eq!(Point2f::lex_cmp(b, a), Ordering::Greater);
        std::assert_eq!(Point2f::lex_cmp(c, a), Ordering::Greater);
        std::assert_eq!(Point2f::lex_cmp(c, b), Ordering::Greater);

        std::assert_eq!(Point2f::lex_cmp(a, a), Ordering::Equal);
    }

    /*#[test]
    fn test_add() {
        std::assert_eq!(
            Point2::new(-9.0, 90.0) + Vec2::new(22.0, -41.0),
            Point2::new(13.0, 49.0)
        );
    }

    #[test]
    fn test_add_assign() {
        let mut a = Point2::new(-9.0, 90.0);
        a += Vec2::new(22.0, -41.0);
        std::assert_eq!(a, Point2::new(13.0, 49.0));
    }

    #[test]
    fn test_sub_point() {
        std::assert_eq!(
            Point2::new(63.0, 31.0) - Point2::new(39.0, 63.0),
            Vec2::new(24.0, -32.0)
        );
    }

    #[test]
    fn test_sub_vec() {
        std::assert_eq!(
            Point2::new(63.0, 31.0) - Vec2::new(39.0, 63.0),
            Point2::new(24.0, -32.0)
        );
    }

    #[test]
    fn test_sub_assign() {
        let mut a = Point2::new(63.0, 31.0);
        a -= Vec2::new(39.0, 63.0);
        std::assert_eq!(a, Point2::new(24.0, -32.0));
    }

    #[test]
    fn test_from_str() {
        std::assert_eq!(
            Point2::from_str("{8.14, 2.98}"),
            Ok(Point2::new(8.14, 2.98))
        );
        std::assert_eq!(
            Point2::from_str("  {4.05, -1.83}"),
            Ok(Point2::new(4.05, -1.83))
        );
        std::assert_eq!(
            Point2::from_str("{-8.89, 6.55}   "),
            Ok(Point2::new(-8.89, 6.55))
        );
        std::assert!(Point2::from_str("not a point").is_err());
    }

    #[test]
    fn test_vec_from_str() {
        std::assert_eq!(
            Point2::vec_from_str("{{7.98, -2.95}, {-8.23, -4.78}, {-2.33, -6.25}}"),
            Ok(vec![
                Point2::new(7.98, -2.95),
                Point2::new(-8.23, -4.78),
                Point2::new(-2.33, -6.25)
            ])
        );
    }*/
}
