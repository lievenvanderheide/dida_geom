use crate::{ScalarDeg1, ScalarDeg2};
use std::ops::{Add, Sub};
use std::cmp::Ordering;

#[derive(Copy, Clone, PartialEq, Eq, Hash)]
pub struct Vec2 {
    x: ScalarDeg1,
    y: ScalarDeg1,
}

impl Vec2 {
    /// Constructs a new Vec2 with the 'ScalarDeg1' coordinates closest to the given 'f64' coordinates.
    pub fn new(x: f64, y: f64) -> Vec2 {
        Vec2 {
            x: ScalarDeg1::new(x),
            y: ScalarDeg1::new(y),
        }
    }

    /// Constructs a vector with the given coordinates.
    pub fn from_coords(x: ScalarDeg1, y: ScalarDeg1) -> Vec2 {
        Vec2 {
            x: x,
            y: y,
        }
    }

    /// Returns the 'x' component of this vector.
    pub fn x(self) -> ScalarDeg1 {
        self.x
    }

    /// Returns the 'y' component of this vector.
    pub fn y(self) -> ScalarDeg1 {
        self.y
    }

    /// Sets the 'x' component of this vector.
    pub fn set_x(&mut self, x: ScalarDeg1) {
        self.x = x;
    }

    /// Sets the 'y' component of this vector.
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
    /// perpendicular of 'a' and the vector 'b'. 
    pub fn cross(a: Vec2, b: Vec2) -> ScalarDeg2 {
        a.x * b.y - a.y * b.x
    }

    /// Returns whether 'a' is lexicographically less than 'b'
    pub fn lex_less_than(a: Vec2, b: Vec2) -> bool {
        a.x < b.x || (a.x == b.x && a.y < b.y)
    }

    /// Returns whether 'a' is lexicographically greater than 'b'.
    pub fn lex_greater_than(a: Vec2, b: Vec2) -> bool {
        a.x > b.x || (a.x == b.x && a.y > b.y)
    }

    /// Lexicographically compares vectors 'a' and 'b'. 
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

impl Sub for Vec2 {
    type Output = Vec2;

    fn sub(self, b: Vec2) -> Vec2 {
        Vec2 {
            x: self.x - b.x,
            y: self.y - b.y,
        }
    }
}

impl std::fmt::Debug for Vec2 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("").field(&self.x).field(&self.y).finish()
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
    fn test_add() {
        std::assert_eq!(Vec2::new(99.0, 4.0) + Vec2::new(93.0, -35.0), Vec2::new(192.0, -31.0));
    }

    #[test]
    fn test_sub() {
        std::assert_eq!(Vec2::new(99.0, 4.0) - Vec2::new(93.0, -35.0), Vec2::new(6.0, 39.0));
    }
}
