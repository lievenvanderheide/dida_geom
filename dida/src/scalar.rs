use std::fmt::{Debug, Formatter, Result};
use std::ops::{Add, AddAssign, Sub, SubAssign, Neg, Mul};
use std::hash::Hash;

pub trait ScalarParams {
    type IntT: Copy + Clone + Add<Output = Self::IntT> + AddAssign + Sub<Output = Self::IntT> + SubAssign + Neg<Output = Self::IntT>;
    
    const DEGREE: usize;
    const DENOM: Self::IntT;
    const QUANTUM: f64;
    const MIN_NUMERATOR: Self::IntT;
    const MAX_NUMERATOR: Self::IntT;

    fn f64_to_int(a: f64) -> Self::IntT;
    fn int_to_f64(a: Self::IntT) -> f64;
}

#[derive(Copy, Clone, Eq, Ord, PartialEq, PartialOrd, Hash)]
pub struct ScalarDeg1Params {}
impl ScalarParams for ScalarDeg1Params {
    type IntT = i32;

    const DEGREE: usize = 1;
    const DENOM: i32 = 4096;
    const QUANTUM: f64 = 1.0 / 4096.0;
    const MIN_NUMERATOR: i32 = i32::MIN;
    const MAX_NUMERATOR: i32 = i32::MAX;

    fn f64_to_int(a: f64) -> i32 {
        a.round_ties_even() as i32
    }

    fn int_to_f64(a: i32) -> f64 {
        a as f64
    }
}

#[derive(Copy, Clone, Eq, Ord, PartialEq, PartialOrd, Hash)]
pub struct ScalarDeg2Params {}
impl ScalarParams for ScalarDeg2Params {
    type IntT = i64;

    const DEGREE: usize = 2;
    const DENOM: i64 = 4096 * 4096;
    const QUANTUM: f64 = 1.0 / (4096.0 * 4096.0);
    const MIN_NUMERATOR: i64 = i64::MIN;
    const MAX_NUMERATOR: i64 = i64::MAX;

    fn f64_to_int(a: f64) -> i64 {
        a.round_ties_even() as i64
    }

    fn int_to_f64(a: i64) -> f64 {
        a as f64
    }
}

/// A fixed point scalar.
///
/// Each scalar has a degree associated with it. Scalars of the same degree can be added and subtracted, resulting in a
/// scalar of the same degree, while scalars of any degree can be multiplied, resulting in a scalar whose degree is the
/// sum of the degree of the two operands. Since the number of bits of a scalar, as well its radix position grow with
/// the degree of the scalar, the result of a multiplication can always be stored exactly; there's no need to shift the
/// result to the correct radix position, and no bits need to be discarded.
#[derive(Clone, Copy, Hash, PartialEq, Eq, PartialOrd, Ord)]
pub struct Scalar<Params: ScalarParams> {
    numerator: Params::IntT,
}

pub type ScalarDeg1 = Scalar<ScalarDeg1Params>;
pub type ScalarDeg2 = Scalar<ScalarDeg2Params>;

impl<Params: ScalarParams> Scalar<Params> {
    /// The denominator of a scalar of this type.
    pub const DENOM: Params::IntT = Params::DENOM;

    /// The difference between consecutive scalars of this type.
    pub const QUANTUM: f64 = Params::QUANTUM;

    /// The lowest value for scalars of this type.
    pub const MIN: Self = Self { numerator: Params::MIN_NUMERATOR };

    /// The highest value for scalars of this type.
    pub const MAX: Self = Self { numerator: Params::MAX_NUMERATOR };

    /// Constructs a scalar with the given numerator.
    ///
    /// The value of the scalar is `numerator / DENOM`.
    pub fn from_numerator(numerator: Params::IntT) -> Self {
        Self { numerator }
    }

    /// Returns the numerator of this scalar.
    pub fn numerator(&self) -> Params::IntT {
        self.numerator
    }
    
    /// Constructs a scalar with the given floating point value.
    ///
    /// The value will be the `ScalarDeg1` closest to `value`.
    pub fn new(value: f64) -> Self {
        Self {
            numerator: Params::f64_to_int((value * Params::int_to_f64(Self::DENOM)).round_ties_even())
        }
    }

    /// Converts this scalar to a `f64` number. Rounds if necessary.
    pub fn as_f64(&self) -> f64 {
        Params::int_to_f64(self.numerator) * Self::QUANTUM
    }
}

impl<Params: ScalarParams> Add for Scalar<Params> {
    type Output = Self;

    fn add(self, b: Self) -> Self {
        Self {
            numerator: self.numerator + b.numerator,
        }
    }
}

impl<Params: ScalarParams> AddAssign for Scalar<Params> {
    fn add_assign(&mut self, b: Self) {
        self.numerator += b.numerator;
    }
}

impl<Params: ScalarParams> Sub for Scalar<Params> {
    type Output = Self;

    fn sub(self, b: Self) -> Self {
        Self {
            numerator: self.numerator - b.numerator,
        }
    }
}

impl<Params: ScalarParams> SubAssign for Scalar<Params> {
    fn sub_assign(&mut self, b: Self) {
        self.numerator -= b.numerator;
    }
}

impl<Params: ScalarParams> Neg for Scalar<Params> {
    type Output = Self;

    fn neg(self) -> Self {
        Self {
            numerator: -self.numerator,
        }
    }
}

impl Mul for ScalarDeg1 {
    type Output = ScalarDeg2;
    
    fn mul(self, b: Self) -> ScalarDeg2 {
        ScalarDeg2 {
            numerator: self.numerator as i64 * b.numerator as i64
        }
    }
}

impl<Params: ScalarParams> Debug for Scalar<Params> {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        self.as_f64().fmt(f)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::cmp::Ordering;

    #[test]
    fn test_from_numerator() {
        std::assert_eq!(ScalarDeg1::from_numerator(123).numerator(), 123);
        std::assert_eq!(ScalarDeg2::from_numerator(123).numerator(), 123);
    }

    #[test]
    fn test_new() {
        std::assert_eq!(ScalarDeg1::new(1.0).numerator(), 4096);
        std::assert_eq!(ScalarDeg1::new(123.4 / 4096.0).numerator(), 123);
        std::assert_eq!(ScalarDeg1::new(123.5 / 4096.0).numerator(), 124);
        std::assert_eq!(ScalarDeg1::new(123456.7 / 4096.0).numerator(), 123457);
        std::assert_eq!(ScalarDeg1::new(-122.5 / 4096.0).numerator(), -122);

        std::assert_eq!(ScalarDeg2::new(1.0).numerator(), 4096 * 4096);
        std::assert_eq!(ScalarDeg2::new(123.4 / (4096.0 * 4096.0)).numerator(), 123);
        std::assert_eq!(ScalarDeg2::new(123.5 / (4096.0 * 4096.0)).numerator(), 124);
        std::assert_eq!(ScalarDeg2::new(123456.7 / (4096.0 * 4096.0)).numerator(), 123457);
        std::assert_eq!(ScalarDeg2::new(-122.5 / (4096.0 * 4096.0)).numerator(), -122);
    }

    #[test]
    fn test_as_f64() {
        std::assert_eq!(ScalarDeg1::new(-9.25).as_f64(), -9.25);
        std::assert_eq!(ScalarDeg2::new(1.5).as_f64(), 1.5);
    }

    #[test]
    fn test_quantum() {
        std::assert_eq!(ScalarDeg1::QUANTUM, ScalarDeg1::from_numerator(1).as_f64());
        std::assert_eq!(ScalarDeg2::QUANTUM, ScalarDeg2::from_numerator(1).as_f64());
    }

    #[test]
    fn test_add() {
        std::assert_eq!(ScalarDeg1::new(-76.0) + ScalarDeg1::new(-415.0), ScalarDeg1::new(-491.0));
        std::assert_eq!(ScalarDeg2::new(-776.0) + ScalarDeg2::new(860.0), ScalarDeg2::new(84.0));
    }

    #[test]
    fn test_add_assign() {
        {
            let mut a = ScalarDeg1::new(-76.0);
            a += ScalarDeg1::new(-415.0);
            std::assert_eq!(a, ScalarDeg1::new(-491.0));
        }

        {
            let mut a = ScalarDeg2::new(-776.0);
            a += ScalarDeg2::new(860.0);
            std::assert_eq!(a, ScalarDeg2::new(84.0));
        }
    }

    #[test]
    fn test_sub() {
        std::assert_eq!(ScalarDeg1::new(-857.0) - ScalarDeg1::new(-348.0), ScalarDeg1::new(-509.0));
        std::assert_eq!(ScalarDeg2::new(677.0) - ScalarDeg2::new(-142.0), ScalarDeg2::new(819.0));
    }

    #[test]
    fn test_sub_assign() {
        {
            let mut a = ScalarDeg1::new(-857.0);
            a -= ScalarDeg1::new(-348.0);
            std::assert_eq!(a, ScalarDeg1::new(-509.0));
        }

        {
            let mut a = ScalarDeg2::new(677.0);
            a -= ScalarDeg2::new(-142.0);
            std::assert_eq!(a, ScalarDeg2::new(819.0));
        }
    }

    #[test]
    fn test_neg() {
        std::assert_eq!(-ScalarDeg1::new(-159.0), ScalarDeg1::new(159.0));
        std::assert_eq!(-ScalarDeg2::new(563.0), ScalarDeg2::new(-563.0));
    }

    #[test]
    fn test_mul() {
        std::assert_eq!(ScalarDeg1::new(62.0) * ScalarDeg1::new(14.0), ScalarDeg2::new(868.0));
    }

    #[test]
    fn test_eq() {
        std::assert!(ScalarDeg1::from_numerator(1) == ScalarDeg1::from_numerator(1));
        std::assert!(!(ScalarDeg1::from_numerator(1) == ScalarDeg1::from_numerator(2)));

        std::assert!(ScalarDeg2::from_numerator(1) == ScalarDeg2::from_numerator(1));
        std::assert!(!(ScalarDeg2::from_numerator(1) == ScalarDeg2::from_numerator(2)));
    }

    #[test]
    fn test_cmp() {
        std::assert_eq!(ScalarDeg1::from_numerator(1).cmp(&ScalarDeg1::from_numerator(2)), Ordering::Less);
        std::assert_eq!(ScalarDeg1::from_numerator(1).cmp(&ScalarDeg1::from_numerator(1)), Ordering::Equal);
        std::assert_eq!(ScalarDeg1::from_numerator(2).cmp(&ScalarDeg1::from_numerator(1)), Ordering::Greater);

        std::assert_eq!(ScalarDeg2::from_numerator(1).cmp(&ScalarDeg2::from_numerator(2)), Ordering::Less);
        std::assert_eq!(ScalarDeg2::from_numerator(1).cmp(&ScalarDeg2::from_numerator(1)), Ordering::Equal);
        std::assert_eq!(ScalarDeg2::from_numerator(2).cmp(&ScalarDeg2::from_numerator(1)), Ordering::Greater);
    }
}