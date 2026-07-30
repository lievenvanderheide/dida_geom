use std::cmp::Ordering;
use std::ops::{Add, AddAssign, Neg, Sub, SubAssign};

/// A 2D vector with `f64` coordinates.
#[derive(Copy, Clone, PartialEq)]
pub struct Vec2f {
    x: f64,
    y: f64,
}

impl Vec2f {
    pub fn new(x: f64, y: f64) -> Self {
        Self { x, y }
    }

    pub fn x(&self) -> f64 {
        self.x
    }

    pub fn y(&self) -> f64 {
        self.y
    }

    /// Sets the `x` component of this vector.
    pub fn set_x(&mut self, x: f64) {
        self.x = x;
    }

    /// Sets the `y` component of this vector.
    pub fn set_y(&mut self, y: f64) {
        self.y = y;
    }

    /// Returns the squared length of this vector.
    pub fn sqr_len(&self) -> f64 {
        self.x * self.x + self.y * self.y
    }

    /// Returns the dot product of 2 vectors.
    pub fn dot(a: Self, b: Self) -> f64 {
        a.x * b.x + a.y * b.y
    }

    /// Returns the 2D cross product of 2 vectors.
    ///
    /// The 2D cross product, also known as the perp dot product, is defined as the dot product of the left
    /// perpendicular of `a` and the vector `b`.
    pub fn cross(a: Self, b: Self) -> f64 {
        a.x * b.y - a.y * b.x
    }

    /// Returns whether `a` is lexicographically less than `b`.
    pub fn lex_less_than(a: Self, b: Self) -> bool {
        a.x < b.x || (a.x == b.x && a.y < b.y)
    }

    /// Returns whether `a` is lexicographically greater than `b`.
    pub fn lex_greater_than(a: Self, b: Self) -> bool {
        a.x > b.x || (a.x == b.x && a.y > b.y)
    }

    /// Lexicographically compares vectors `a` and `b`.
    pub fn lex_cmp(a: Self, b: Self) -> Ordering {
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

impl Add for Vec2f {
    type Output = Vec2f;

    fn add(self, b: Vec2f) -> Vec2f {
        Vec2f {
            x: self.x + b.x,
            y: self.y + b.y,
        }
    }
}

impl AddAssign for Vec2f {
    fn add_assign(&mut self, b: Vec2f) {
        self.x += b.x;
        self.y += b.y;
    }
}

impl Sub for Vec2f {
    type Output = Vec2f;

    fn sub(self, b: Vec2f) -> Vec2f {
        Vec2f {
            x: self.x - b.x,
            y: self.y - b.y,
        }
    }
}

impl SubAssign for Vec2f {
    fn sub_assign(&mut self, b: Vec2f) {
        self.x -= b.x;
        self.y -= b.y;
    }
}

impl Neg for Vec2f {
    type Output = Vec2f;

    fn neg(self) -> Vec2f {
        Vec2f {
            x: -self.x,
            y: -self.y,
        }
    }
}

impl std::fmt::Debug for Vec2f {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_tuple("").field(&self.x).field(&self.y).finish()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new() {
        let vec = Vec2f::new(28.2, 57.1);
        std::assert_eq!(vec.x(), 28.2);
        std::assert_eq!(vec.y(), 57.1);
    }

    #[test]
    fn test_set_x() {
        let mut vec = Vec2f::new(8.3, -9.6);
        vec.set_x(8.7);
        std::assert_eq!(vec, Vec2f::new(8.7, -9.6));
    }

    #[test]
    fn test_set_y() {
        let mut vec = Vec2f::new(8.3, -9.6);
        vec.set_y(8.7);
        std::assert_eq!(vec, Vec2f::new(8.3, 8.7));
    }

    #[test]
    fn test_sqr_len() {
        let vec = Vec2f::new(5.0, 2.0);
        std::assert_eq!(vec.sqr_len(), 29.0);
    }

    #[test]
    fn test_dot() {
        std::assert_eq!(Vec2f::dot(Vec2f::new(3.0, 2.0), Vec2f::new(5.0, -7.0)), 1.0);
    }

    #[test]
    fn test_cross() {
        std::assert_eq!(
            Vec2f::cross(Vec2f::new(3.0, 2.0), Vec2f::new(7.0, 5.0)),
            1.0
        );
    }

    #[test]
    fn test_lex_less_than() {
        let a = Vec2f::new(1.0, 2.0);
        let b = Vec2f::new(1.0, 3.0);
        let c = Vec2f::new(3.0, 4.0);

        std::assert!(Vec2f::lex_less_than(a, b));
        std::assert!(Vec2f::lex_less_than(a, c));
        std::assert!(Vec2f::lex_less_than(b, c));

        std::assert!(!Vec2f::lex_less_than(b, a));
        std::assert!(!Vec2f::lex_less_than(c, a));
        std::assert!(!Vec2f::lex_less_than(c, b));

        std::assert!(!Vec2f::lex_less_than(a, a));
    }

    #[test]
    fn test_lex_greater_than() {
        let a = Vec2f::new(1.0, 2.0);
        let b = Vec2f::new(1.0, 3.0);
        let c = Vec2f::new(3.0, 4.0);

        std::assert!(!Vec2f::lex_greater_than(a, b));
        std::assert!(!Vec2f::lex_greater_than(a, c));
        std::assert!(!Vec2f::lex_greater_than(b, c));

        std::assert!(Vec2f::lex_greater_than(b, a));
        std::assert!(Vec2f::lex_greater_than(c, a));
        std::assert!(Vec2f::lex_greater_than(c, b));

        std::assert!(!Vec2f::lex_greater_than(a, a));
    }

    #[test]
    fn test_lex_cmp() {
        let a = Vec2f::new(1.0, 2.0);
        let b = Vec2f::new(1.0, 3.0);
        let c = Vec2f::new(3.0, 4.0);

        std::assert_eq!(Vec2f::lex_cmp(a, b), Ordering::Less);
        std::assert_eq!(Vec2f::lex_cmp(a, c), Ordering::Less);
        std::assert_eq!(Vec2f::lex_cmp(b, c), Ordering::Less);

        std::assert_eq!(Vec2f::lex_cmp(b, a), Ordering::Greater);
        std::assert_eq!(Vec2f::lex_cmp(c, a), Ordering::Greater);
        std::assert_eq!(Vec2f::lex_cmp(c, b), Ordering::Greater);

        std::assert_eq!(Vec2f::lex_cmp(a, a), Ordering::Equal);
    }

    #[test]
    fn test_add() {
        std::assert_eq!(
            Vec2f::new(99.0, 4.0) + Vec2f::new(93.0, -35.0),
            Vec2f::new(192.0, -31.0)
        );
    }

    #[test]
    fn test_add_assign() {
        let mut a = Vec2f::new(99.0, 4.0);
        a += Vec2f::new(93.0, -35.0);
        std::assert_eq!(a, Vec2f::new(192.0, -31.0));
    }

    #[test]
    fn test_sub() {
        std::assert_eq!(
            Vec2f::new(99.0, 4.0) - Vec2f::new(93.0, -35.0),
            Vec2f::new(6.0, 39.0)
        );
    }

    #[test]
    fn test_sub_assign() {
        let mut a = Vec2f::new(99.0, 4.0);
        a -= Vec2f::new(93.0, -35.0);
        std::assert_eq!(a, Vec2f::new(6.0, 39.0));
    }

    #[test]
    fn test_neg() {
        let vec = Vec2f::new(1.7, -1.2);
        std::assert_eq!(-vec, Vec2f::new(-1.7, 1.2));
    }
}
