use crate::{ScalarDeg1, Vec2};
use crate::parser::parser::Parser;
use crate::parser::geometry_parsers::{parse_point2, parse_point2_vec};
use std::ops::{Add, AddAssign, Sub, SubAssign};
use std::cmp::Ordering;
use std::str::FromStr;

/// A 2D vector with [`ScalarDeg1`] coordinates.
#[derive(Copy, Clone, PartialEq, Eq, Hash)]
pub struct Point2 {
    pos: Vec2,
}

impl Point2 {
    /// Constructs a `Point2` with the `ScalarDeg1` coordinates closest to the given `f64` coordinates.
    pub fn new(x: f64, y: f64) -> Point2 {
        Point2 {
            pos: Vec2::new(x, y),
        }
    }

    /// Constructs a `Point2` with the given coordinates.
    pub fn from_coords(x: ScalarDeg1, y: ScalarDeg1) -> Point2 {
        Point2 {
            pos: Vec2::from_coords(x, y),
        }
    }

    pub fn vec_from_str(s: &str) -> Result<Vec<Point2>, String> {
        let mut parser = Parser::new(s);

        parser.skip_optional_whitespace();
        let Some(result) = parse_point2_vec(&mut parser) else {
            return Err(format!("Failed to parse point \"{}\"", s));
        };

        parser.skip_optional_whitespace();
        if !parser.has_finished() {
            return Err(format!("Failed to parse point \"{}\"", s));
        }

        Ok(result)
    }

    /// Constructs a `Point2` from a `Vec2`.
    pub fn from_vec2(pos: Vec2) -> Point2 {
        Point2 { pos }
    }

    /// Returns the `x` coordinate of this point.
    pub fn x(self) -> ScalarDeg1 {
        self.pos.x()
    }

    /// Returns the `y` coordinate of this point.
    pub fn y(self) -> ScalarDeg1 {
        self.pos.y()
    }

    /// Sets the `x` coordinate of this point.
    pub fn set_x(&mut self, x: ScalarDeg1) {
        self.pos.set_x(x);
    }

    /// Sets the `y` coordinate of this point.
    pub fn set_y(&mut self, y: ScalarDeg1) {
        self.pos.set_y(y);
    }

    /// Returns whether `a` is lexicographically less than `b`.
    pub fn lex_less_than(a: Point2, b: Point2) -> bool {
        Vec2::lex_less_than(a.pos, b.pos)
    }

    /// Returns whether `a` is lexicographically greater than `b`.
    pub fn lex_greater_than(a: Point2, b: Point2) -> bool {
        Vec2::lex_greater_than(a.pos, b.pos)
    }

    /// Lexicographically compares vectors `a` and `b`.
    pub fn lex_cmp(a: Point2, b: Point2) -> Ordering {
        Vec2::lex_cmp(a.pos, b.pos)
    }
}

impl Add<Vec2> for Point2 {
    type Output = Point2;

    fn add(self, b: Vec2) -> Point2 {
        Point2 {
            pos: self.pos + b,
        }
    }
}

impl AddAssign<Vec2> for Point2 {
    fn add_assign(&mut self, b: Vec2) {
        self.pos += b;
    }
}

impl Sub for Point2 {
    type Output = Vec2;

    fn sub(self, b: Point2) -> Vec2 {
        self.pos - b.pos
    }
}

impl Sub<Vec2> for Point2 {
    type Output = Point2;

    fn sub(self, b: Vec2) -> Point2 {
        Point2 {
            pos: self.pos - b,
        }
    }
}

impl SubAssign<Vec2> for Point2 {
    fn sub_assign(&mut self, b: Vec2) {
        self.pos -= b;
    }
}

impl std::fmt::Debug for Point2 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        self.pos.fmt(f)
    }
}

impl FromStr for Point2 {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, String> {
        let mut parser = Parser::new(s);

        parser.skip_optional_whitespace();
        let Some(result) = parse_point2(&mut parser) else {
            return Err(format!("Failed to parse point \"{}\"", s));
        };

        parser.skip_optional_whitespace();
        if !parser.has_finished() {
            return Err(format!("Failed to parse point \"{}\"", s));
        }

        Ok(result)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new() {
        let point = Point2::new(8.4, -5.6);
        std::assert_eq!(point.x(), ScalarDeg1::new(8.4));
        std::assert_eq!(point.y(), ScalarDeg1::new(-5.6));
    }

    #[test]
    fn test_from_coords() {
        let point = Point2::from_coords(ScalarDeg1::new(8.4), ScalarDeg1::new(-5.6));
        std::assert_eq!(point.x(), ScalarDeg1::new(8.4));
        std::assert_eq!(point.y(), ScalarDeg1::new(-5.6));
    }

    #[test]
    fn from_vec2() {
        let point = Point2::from_vec2(Vec2::new(6.4, -4.0));
        std::assert_eq!(point.x(), ScalarDeg1::new(6.4));
        std::assert_eq!(point.y(), ScalarDeg1::new(-4.0));
    }

    #[test]
    fn test_set_x() {
        let mut point = Point2::new(7.1, 8.0);
        point.set_x(ScalarDeg1::new(-6.0));
        std::assert_eq!(point, Point2::new(-6.0, 8.0));
    }

    #[test]
    fn test_set_y() {
        let mut point = Point2::new(8.4, 3.4);
        point.set_y(ScalarDeg1::new(-4.9));
        std::assert_eq!(point, Point2::new(8.4, -4.9));
    }

    #[test]
    fn test_lex_less_than() {
        let a = Point2::new(1.0, 2.0);
        let b = Point2::new(1.0, 3.0);
        let c = Point2::new(3.0, 4.0);

        std::assert!(Point2::lex_less_than(a, b));
        std::assert!(Point2::lex_less_than(a, c));
        std::assert!(Point2::lex_less_than(b, c));

        std::assert!(!Point2::lex_less_than(b, a));
        std::assert!(!Point2::lex_less_than(c, a));
        std::assert!(!Point2::lex_less_than(c, b));

        std::assert!(!Point2::lex_less_than(a, a));
    }

    #[test]
    fn test_lex_greater_than() {
        let a = Point2::new(1.0, 2.0);
        let b = Point2::new(1.0, 3.0);
        let c = Point2::new(3.0, 4.0);

        std::assert!(!Point2::lex_greater_than(a, b));
        std::assert!(!Point2::lex_greater_than(a, c));
        std::assert!(!Point2::lex_greater_than(b, c));

        std::assert!(Point2::lex_greater_than(b, a));
        std::assert!(Point2::lex_greater_than(c, a));
        std::assert!(Point2::lex_greater_than(c, b));

        std::assert!(!Point2::lex_greater_than(a, a));
    }

    #[test]
    fn test_lex_cmp() {
        let a = Point2::new(1.0, 2.0);
        let b = Point2::new(1.0, 3.0);
        let c = Point2::new(3.0, 4.0);

        std::assert_eq!(Point2::lex_cmp(a, b), Ordering::Less);
        std::assert_eq!(Point2::lex_cmp(a, c), Ordering::Less);
        std::assert_eq!(Point2::lex_cmp(b, c), Ordering::Less);

        std::assert_eq!(Point2::lex_cmp(b, a), Ordering::Greater);
        std::assert_eq!(Point2::lex_cmp(c, a), Ordering::Greater);
        std::assert_eq!(Point2::lex_cmp(c, b), Ordering::Greater);

        std::assert_eq!(Point2::lex_cmp(a, a), Ordering::Equal);
    }

    #[test]
    fn test_add() {
        std::assert_eq!(Point2::new(-9.0, 90.0) + Vec2::new(22.0, -41.0), Point2::new(13.0, 49.0));
    }

    #[test]
    fn test_add_assign() {
        let mut a = Point2::new(-9.0, 90.0);
        a += Vec2::new(22.0, -41.0);
        std::assert_eq!(a, Point2::new(13.0, 49.0));
    }

    #[test]
    fn test_sub_point() {
        std::assert_eq!(Point2::new(63.0, 31.0) - Point2::new(39.0, 63.0), Vec2::new(24.0, -32.0));
    }

    #[test]
    fn test_sub_vec() {
        std::assert_eq!(Point2::new(63.0, 31.0) - Vec2::new(39.0, 63.0), Point2::new(24.0, -32.0));
    }

    #[test]
    fn test_sub_assign() {
        let mut a = Point2::new(63.0, 31.0);
        a -= Vec2::new(39.0, 63.0);
        std::assert_eq!(a, Point2::new(24.0, -32.0));
    }

    #[test]
    fn test_from_str() {
        std::assert_eq!(Point2::from_str("{8.14, 2.98}"), Ok(Point2::new(8.14, 2.98)));    
        std::assert_eq!(Point2::from_str("  {4.05, -1.83}"), Ok(Point2::new(4.05, -1.83)));
        std::assert_eq!(Point2::from_str("{-8.89, 6.55}   "), Ok(Point2::new(-8.89, 6.55)));
        std::assert!(Point2::from_str("not a point").is_err());
    }

    #[test]
    fn test_vec_from_str() {
        std::assert_eq!(
            Point2::vec_from_str("{{7.98, -2.95}, {-8.23, -4.78}, {-2.33, -6.25}}"),
            Ok(vec![Point2::new(7.98, -2.95), Point2::new(-8.23, -4.78), Point2::new(-2.33, -6.25)])
        );
    }
}
