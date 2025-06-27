use crate::{ScalarDeg1, ScalarDeg2};
use std::ops::{Add, AddAssign, Sub, SubAssign};
use std::cmp::Ordering;
use std::str::FromStr;
use crate::parser::parser::Parser;
use crate::parser::geometry_parsers::parse_vec2;

/// A 2D vector with [`ScalarDeg1`] coordinates.
#[derive(Copy, Clone, PartialEq, Eq, Hash)]
pub struct Vec2 {
    x: ScalarDeg1,
    y: ScalarDeg1,
}

impl Vec2 {
    /// Constructs a `Vec2` with the `ScalarDeg1` coordinates closest to the given `f64` coordinates.
    pub fn new(x: f64, y: f64) -> Vec2 {
        Vec2 {
            x: ScalarDeg1::new(x),
            y: ScalarDeg1::new(y),
        }
    }

    /// Constructs a `Vec2` with the given coordinates.
    pub fn from_coords(x: ScalarDeg1, y: ScalarDeg1) -> Vec2 {
        Vec2 {
            x: x,
            y: y,
        }
    }

    /// Returns the `x` component of this vector.
    pub fn x(self) -> ScalarDeg1 {
        self.x
    }

    /// Returns the `y` component of this vector.
    pub fn y(self) -> ScalarDeg1 {
        self.y
    }

    /// Sets the `x` component of this vector.
    pub fn set_x(&mut self, x: ScalarDeg1) {
        self.x = x;
    }

    /// Sets the `y` component of this vector.
    pub fn set_y(&mut self, y: ScalarDeg1) {
        self.y = y;
    }

    /// Returns the dot product of 2 vectors.
    pub fn dot(a: Vec2, b: Vec2) -> ScalarDeg2 {
        a.x * b.x + a.y * b.y
    }

    /// Returns the 2D cross product of 2 vectors.
    ///
    /// The 2D cross product, also known as the perp dot product, is defined as the dot product of the left
    /// perpendicular of `a` and the vector `b`. 
    pub fn cross(a: Vec2, b: Vec2) -> ScalarDeg2 {
        a.x * b.y - a.y * b.x
    }

    /// Returns whether `a` is lexicographically less than `b`.
    pub fn lex_less_than(a: Vec2, b: Vec2) -> bool {
        a.x < b.x || (a.x == b.x && a.y < b.y)
    }

    /// Returns whether `a` is lexicographically greater than `b`.
    pub fn lex_greater_than(a: Vec2, b: Vec2) -> bool {
        a.x > b.x || (a.x == b.x && a.y > b.y)
    }

    /// Lexicographically compares vectors `a` and `b`.
    pub fn lex_cmp(a: Vec2, b: Vec2) -> Ordering {
        if a.x < b.x {
            Ordering::Less
        } else if a.x > b.x {
            Ordering::Greater
        } else {
            if a.y < b.y {
                Ordering::Less
            } else if a.y > b.y {
                Ordering::Greater
            } else {
                Ordering::Equal
            }
        }
    }

    /// Rotates this vector by 90 degrees in the counterclockwise direction and returns the result.
    pub fn left_perpendicular(&self) -> Vec2 {
        Vec2 {
            x: -self.y,
            y: self.x,
        }
    }

    /// Rotates this vector by 90 degrees in the clockwise direction and returns the result.    
    pub fn right_perpendicular(&self) -> Vec2 {
        Vec2 {
            x: self.y,
            y: -self.x,
        }
    }
}

impl Add for Vec2 {
    type Output = Vec2;

    fn add(self, b: Vec2) -> Vec2 {
        Vec2 {
            x: self.x + b.x,
            y: self.y + b.y,
        }
    }
}

impl AddAssign for Vec2 {
    fn add_assign(&mut self, b: Vec2) {
        self.x += b.x;
        self.y += b.y;
    }
}

impl Sub for Vec2 {
    type Output = Vec2;

    fn sub(self, b: Vec2) -> Vec2 {
        Vec2 {
            x: self.x - b.x,
            y: self.y - b.y,
        }
    }
}

impl SubAssign for Vec2 {
    fn sub_assign(&mut self, b: Vec2) {
        self.x -= b.x;
        self.y -= b.y;
    }
}

impl std::fmt::Debug for Vec2 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("").field(&self.x).field(&self.y).finish()
    }
}

impl FromStr for Vec2 {
    type Err = String;

    fn from_str(s: &str) -> Result<Vec2, String> {
        let mut parser = Parser::new(s);

        parser.skip_optional_whitespace();
        let Some(result) = parse_vec2(&mut parser) else {
            return Err(format!("Failed to parse vector \"{}\"", s));
        };

        parser.skip_optional_whitespace();
        if !parser.has_finished() {
            return Err(format!("Failed to parse vector \"{}\"", s));
        }

        Ok(result)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new() {
        let vec = Vec2::new(28.2, 57.1);
        std::assert_eq!(vec.x(), ScalarDeg1::new(28.2));
        std::assert_eq!(vec.y(), ScalarDeg1::new(57.1));
    }

    #[test]
    fn test_from_coords() {
        let vec = Vec2::from_coords(ScalarDeg1::new(28.2), ScalarDeg1::new(57.1));
        std::assert_eq!(vec.x(), ScalarDeg1::new(28.2));
        std::assert_eq!(vec.y(), ScalarDeg1::new(57.1));
    }

    #[test]
    fn test_set_x() {
        let mut vec = Vec2::new(8.3, -9.6);
        vec.set_x(ScalarDeg1::new(8.7));
        std::assert_eq!(vec, Vec2::new(8.7, -9.6));
    }

    #[test]
    fn test_set_y() {
        let mut vec = Vec2::new(8.3, -9.6);
        vec.set_y(ScalarDeg1::new(8.7));
        std::assert_eq!(vec, Vec2::new(8.3, 8.7));
    }

    #[test]
    fn test_dot() {
        std::assert_eq!(Vec2::dot(Vec2::new(3.0, 2.0), Vec2::new(5.0, -7.0)), ScalarDeg2::new(1.0));
    }

    #[test]
    fn test_cross() {
        std::assert_eq!(Vec2::cross(Vec2::new(3.0, 2.0), Vec2::new(7.0, 5.0)), ScalarDeg2::new(1.0));
    }

    #[test]
    fn test_lex_less_than() {
        let a = Vec2::new(1.0, 2.0);
        let b = Vec2::new(1.0, 3.0);
        let c = Vec2::new(3.0, 4.0);
        
        std::assert!(Vec2::lex_less_than(a, b));
        std::assert!(Vec2::lex_less_than(a, c));
        std::assert!(Vec2::lex_less_than(b, c));

        std::assert!(!Vec2::lex_less_than(b, a));
        std::assert!(!Vec2::lex_less_than(c, a));
        std::assert!(!Vec2::lex_less_than(c, b));

        std::assert!(!Vec2::lex_less_than(a, a));
    }

    #[test]
    fn test_lex_greater_than() {
        let a = Vec2::new(1.0, 2.0);
        let b = Vec2::new(1.0, 3.0);
        let c = Vec2::new(3.0, 4.0);

        std::assert!(!Vec2::lex_greater_than(a, b));
        std::assert!(!Vec2::lex_greater_than(a, c));
        std::assert!(!Vec2::lex_greater_than(b, c));

        std::assert!(Vec2::lex_greater_than(b, a));
        std::assert!(Vec2::lex_greater_than(c, a));
        std::assert!(Vec2::lex_greater_than(c, b));

        std::assert!(!Vec2::lex_greater_than(a, a));
    }

    #[test]
    fn test_lex_cmp() {
        let a = Vec2::new(1.0, 2.0);
        let b = Vec2::new(1.0, 3.0);
        let c = Vec2::new(3.0, 4.0);

        std::assert_eq!(Vec2::lex_cmp(a, b), Ordering::Less);
        std::assert_eq!(Vec2::lex_cmp(a, c), Ordering::Less);
        std::assert_eq!(Vec2::lex_cmp(b, c), Ordering::Less);

        std::assert_eq!(Vec2::lex_cmp(b, a), Ordering::Greater);
        std::assert_eq!(Vec2::lex_cmp(c, a), Ordering::Greater);
        std::assert_eq!(Vec2::lex_cmp(c, b), Ordering::Greater);

        std::assert_eq!(Vec2::lex_cmp(a, a), Ordering::Equal);
    }

    #[test]
    fn test_left_perpendicular() {
        let a = Vec2::new(f64::cos(1.0), f64::sin(1.0));
        let expected_result = Vec2::new(
            f64::cos(1.0 + std::f64::consts::FRAC_PI_2),
            f64::sin(1.0 + std::f64::consts::FRAC_PI_2)
        );
        std::assert_eq!(a.left_perpendicular(), expected_result);
    }

    #[test]
    fn test_right_perpendicular() {
        let a = Vec2::new(f64::cos(1.0), f64::sin(1.0));
        let expected_result = Vec2::new(
            f64::cos(1.0 - std::f64::consts::FRAC_PI_2),
            f64::sin(1.0 - std::f64::consts::FRAC_PI_2)
        );
        std::assert_eq!(a.right_perpendicular(), expected_result);
    }

    #[test]
    fn test_add() {
        std::assert_eq!(Vec2::new(99.0, 4.0) + Vec2::new(93.0, -35.0), Vec2::new(192.0, -31.0));
    }

    #[test]
    fn test_add_assign() {
        let mut a = Vec2::new(99.0, 4.0);
        a += Vec2::new(93.0, -35.0);
        std::assert_eq!(a, Vec2::new(192.0, -31.0));
    }

    #[test]
    fn test_sub() {
        std::assert_eq!(Vec2::new(99.0, 4.0) - Vec2::new(93.0, -35.0), Vec2::new(6.0, 39.0));
    }

    #[test]
    fn test_sub_assign() {
        let mut a = Vec2::new(99.0, 4.0);
        a -= Vec2::new(93.0, -35.0);
        std::assert_eq!(a, Vec2::new(6.0, 39.0));
    }

    #[test]
    fn test_from_str() {
        std::assert_eq!(Vec2::from_str("{1.32, 7.18}"), Ok(Vec2::new(1.32, 7.18)));    
        std::assert_eq!(Vec2::from_str("  {9.19, -6.07}"), Ok(Vec2::new(9.19, -6.07)));
        std::assert_eq!(Vec2::from_str("{-8.58, 7.52}   "), Ok(Vec2::new(-8.58, 7.52)));
        std::assert!(Vec2::from_str("not a point").is_err());
    }
}
