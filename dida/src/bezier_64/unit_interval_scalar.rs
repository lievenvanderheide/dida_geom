use crate::ScalarDeg1;
use std::ops::{Add, Mul, Sub};
use std::fmt::Debug;

/// A 64-bit fixed point scalar in the interval [0, 1).
///
/// This scalar type is intended to be used when exact arithmetic is not feasible so rounding is necessary.
///
/// To determine the error bound of a computation, it's useful to associate an (implicit) 'error number' with each
/// variable. Given an approximation and it's error number, the following error bound holds:
///
///   abs(a - scalar_approx(a)) <= error_number(a) * QUANTUM.
///
/// Where `a` is the value the variable would have if the whole computation had been exact, while `scalar_approx(a)` is
/// the value actually computed.
///
/// The error number of the result of the basic operations can be computed as follows.
///
///   error_number(a + b) = max(error_number(a), error_number(b))
///   error_number(a - b) = max(error_number(a), error_number(b))
///   error_number(a * b) = error_number(a) + error_number(b) + 1
///
/// Similar rules for other operations are documented at the respective function.
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default)]
pub struct UnitIntervalScalar64 {
    /// The numerator of this fixed point scalar. The denominator is implicitly 2^64.
    numerator: u64,
}

impl UnitIntervalScalar64 {
    /// The minimum representable scalar.
    pub const MIN: UnitIntervalScalar64 = UnitIntervalScalar64 { numerator: u64::MIN };

    /// The maximum representable scalar.
    pub const MAX: UnitIntervalScalar64 = UnitIntervalScalar64 { numerator: u64::MAX };

    /// The distance between two consecutive representable scalars.
    pub const QUANTUM: UnitIntervalScalar64 = UnitIntervalScalar64 { numerator: 1 };

    /// Constructs a scalar with the given `f64` value.
    ///
    /// Since `UnitIntervalScalar64` values have higher precision than `f64`, this conversion is exact. The error number
    /// of the resulting value is thus 0.
    ///
    /// It's a precondition that 0.0 <= value < 1.0.
    pub fn new(value: f64) -> UnitIntervalScalar64 {
        std::assert!(value >= 0.0 && value < 1.0);
        
        let one_numerator = (1_u128 << 64) as f64;
        UnitIntervalScalar64 {
            numerator: (value * one_numerator) as u64,
        }
    }

    /// Converts this scalar to a `f64` number. Rounds if necessary.
    pub fn as_f64(self) -> f64 {
        let one_numerator = (1_u128 << 64) as f64;
        (self.numerator as f64) * (1.0 / one_numerator)
    }

    /// Constructs a scalar from the given `ScalarDeg1` such that the full range of `ScalarDeg1` is (roughly) mapped to
    /// the full range of `UnitIntervalScalar64`.
    ///
    /// It's only roughly the full range, because `ScalarDeg1::MAX` won't map to exactly `UnitIntervalScalar64::MAX`,
    /// instead we use the mapping such that `ScalarDeg1::MAX + ScalarDeg1::QUANTUM` would map to
    /// `UnitIntervalScalar64::MAX + UnitIntervalScalar64::QUANTUM` if both values were in range.
    ///
    /// Since `UnitIntervalScalar64` values have higher precision than `ScalarDeg1`, this conversion is exact. The error
    /// number of the resulting value is thus 0.
    pub fn from_scalar_deg_1(value: ScalarDeg1) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: (value.numerator().wrapping_add(i32::MIN).cast_unsigned() as u64) << 32
        }
    }

    /// Converts this scalar to a `ScalarDeg1` using the same mapping as `from_scalar_deg_1`. Rounds to nearest if
    /// necessary.
    pub fn to_scalar_deg_1(self) -> ScalarDeg1 {
        let round_to_nearest_offset = (1 as u64) << 31;
        let rounded = ((self.numerator + round_to_nearest_offset) >> 32) as u32;
        ScalarDeg1::from_numerator(rounded.cast_signed().wrapping_sub(i32::MIN))
    }

    /// Constructs a `UnitIntervalScalar64` with the given numerator. The new scalar will have the value
    /// `numerator / 2^64`.
    pub fn from_numerator(numerator: u64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 { numerator }
    }

    /// Returns the numerator of this `UnitIntervalScalar64`.
    pub fn numerator(&self) -> u64 {
        self.numerator
    }

    /// Multiplies this scalar by 0.5 and returns the result.
    ///
    /// error_number(a.half()) = error_number(a) + 1
    pub fn half(self) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator >> 1,
        }
    }

    /// Returns the midpoint between two scalars.
    ///
    /// error_number(a.half()) = max(error_number(a), error_number(b)) + 1
    pub fn midpoint(a: UnitIntervalScalar64, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: a.numerator.midpoint(b.numerator),
        }
    }

    /// Linearly interpolates between `a` and `b` using parameter `t`.
    ///
    /// error_number(interpolate(a, b, t)) = error_number(t) + max(error_number(a), error_number(b)) + 1
    pub fn interpolate(a: UnitIntervalScalar64, b: UnitIntervalScalar64, t: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        if a < b {
            a + t * (b - a)
        } else {
            a - t * (a - b)
        }
    }

    /// Returns the interpolation parameter `t` such that `interpolate(a, b, t) == c`.
    ///
    /// It's a precondition that `c` lies in the interval `[a, b]`.
    ///
    /// If the inputs are exact, then the result will be off by at most QUANTUM (so the error number is 1). If the
    /// inputs may already contain error, then the final error is unbounded. It's not possible to give the typical
    /// formula for the error number in terms of the error number of the inputs. To see why this is not possible,
    /// consider the case where the actual values of a, b, c are 0, QUANTUM and 0, with a, b exact and c an error number
    /// of 1. The result of this function will be 0. A possible exact value corresponding to c is QUANTUM though, which
    /// means the exact result may be 1.
    pub fn interpolation_param(a: UnitIntervalScalar64, b: UnitIntervalScalar64, c: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        let num;
        let denom;
        if a < b {
            std::debug_assert!(a <= c && c <= b);
            num = c - a;
            denom = b - a;
        } else {
            std::debug_assert!(b <= c && c <= a);
            num = a - c;
            denom = a - b;
        }

        if num == denom {
            UnitIntervalScalar64::MAX
        } else {
            UnitIntervalScalar64 {
                numerator: (((num.numerator as u128) << 64) / (denom.numerator as u128)) as u64,
            }
        }
    }

    /// Adds `self` and `b`, while wrapping around if the result is >= 1.0, then returns the result.
    pub fn wrapping_add(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator.wrapping_add(b.numerator),
        }
    }

    /// Subtracts `b` from `self`, while wrapping around if the result is < 0.0, then returns the result.
    pub fn wrapping_sub(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator.wrapping_sub(b.numerator),
        }
    }

    /// Adds `self` and `b`, clamps the result if it exceeds the `UnitIntervalScalar64` range, then returns the result.
    pub fn saturating_add(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator.saturating_add(b.numerator),
        }
    }

    /// Subtracts `b` from `self`, clamps the result if it exceeds the `UnitIntervalScalar64` range, then returns the
    /// result.
    pub fn saturating_sub(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator.saturating_sub(b.numerator),
        }
    }

    /// Compares two scalars with a tolerance.
    ///
    /// More precisely: Given `UnitIntervalScalar64` approximations `a` and `b` and `error_number = error_number(a) +
    /// error_number(b)`, returns whether the interval of possible exact values for `a` and `b` overlap.
    pub fn equal_within_tolerance(a: UnitIntervalScalar64, b: UnitIntervalScalar64, error_number: usize) -> bool {
        let tolerance = UnitIntervalScalar64 {
            numerator: error_number as u64
        };

        return a.saturating_sub(tolerance) <= b && b <= a.saturating_add(tolerance);
    }
}

impl Add for UnitIntervalScalar64 {
    type Output = UnitIntervalScalar64;

    fn add(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator + b.numerator
        }
    }
}

impl Sub for UnitIntervalScalar64 {
    type Output = UnitIntervalScalar64;

    fn sub(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        UnitIntervalScalar64 {
            numerator: self.numerator - b.numerator
        }
    }
}

impl Mul for UnitIntervalScalar64 {
    type Output = UnitIntervalScalar64;

    fn mul(self, b: UnitIntervalScalar64) -> UnitIntervalScalar64 {
        let a_u128 = self.numerator as u128;
        let b_u128 = b.numerator as u128;
        UnitIntervalScalar64 {
            numerator: ((a_u128 * b_u128) >> 64) as u64
        }
    }
}

impl Debug for UnitIntervalScalar64 {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        Debug::fmt(&self.as_f64(), f)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_from_to_f64() {
        std::assert_eq!(UnitIntervalScalar64::new(0.759).as_f64(), 0.759);
        std::assert_eq!(UnitIntervalScalar64::new(0.261).as_f64(), 0.261);
    }

    #[test]
    fn test_from_to_scalar_deg_1() {
        let a = ScalarDeg1::new(-722.366);
        std::assert_eq!(UnitIntervalScalar64::from_scalar_deg_1(a).to_scalar_deg_1(), a);
        std::assert_eq!(UnitIntervalScalar64::from_scalar_deg_1(ScalarDeg1::MIN).to_scalar_deg_1(), ScalarDeg1::MIN);
        std::assert_eq!(UnitIntervalScalar64::from_scalar_deg_1(ScalarDeg1::MAX).to_scalar_deg_1(), ScalarDeg1::MAX);

        std::assert_eq!(UnitIntervalScalar64::from_scalar_deg_1(ScalarDeg1::MIN), UnitIntervalScalar64::MIN);
        std::assert_eq!(
            UnitIntervalScalar64::from_scalar_deg_1(ScalarDeg1::MAX),
            UnitIntervalScalar64::from_numerator(0xffffffff00000000)
        );
    }

    #[test]
    fn test_from_to_numerator() {
        std::assert_eq!(UnitIntervalScalar64::from_numerator(70965).numerator(), 70965);
    }

    #[test]
    fn test_half() {
        std::assert_eq!(UnitIntervalScalar64::new(0.4).half(), UnitIntervalScalar64::new(0.2));
    }

    #[test]
    fn test_midpoint() {
        std::assert_eq!(
            UnitIntervalScalar64::midpoint(UnitIntervalScalar64::new(0.25), UnitIntervalScalar64::new(0.875)),
            UnitIntervalScalar64::new(0.5625),
        );
    }

    #[test]
    fn test_interpolate() {
        // a < b
        std::assert_eq!(
            UnitIntervalScalar64::interpolate(
                UnitIntervalScalar64::new(0.3125),
                UnitIntervalScalar64::new(0.8125),
                UnitIntervalScalar64::new(0.1875)
            ),
            UnitIntervalScalar64::new(0.40625)
        );

        // a > b
        std::assert_eq!(
            UnitIntervalScalar64::interpolate(
                UnitIntervalScalar64::new(0.875),
                UnitIntervalScalar64::new(0.25),
                UnitIntervalScalar64::new(0.6875)
            ),
            UnitIntervalScalar64::new(0.4453125)
        );
    }

    #[test]
    fn test_interpolation_param() {
        // a < b
        std::assert_eq!(
            UnitIntervalScalar64::interpolation_param(
                UnitIntervalScalar64::new(0.3125),
                UnitIntervalScalar64::new(0.8125),
                UnitIntervalScalar64::new(0.40625)
            ),
            UnitIntervalScalar64::new(0.1875)
        );

        // a > b
        std::assert_eq!(
            UnitIntervalScalar64::interpolation_param(
                UnitIntervalScalar64::new(0.875),
                UnitIntervalScalar64::new(0.25),
                UnitIntervalScalar64::new(0.4453125),
            ),
            UnitIntervalScalar64::new(0.6875)
        );

        // Interpolation param of 1.0 (which is rounded to UnitIntervalScalar64::MAX).
        std::assert_eq!(
            UnitIntervalScalar64::interpolation_param(
                UnitIntervalScalar64::new(0.84),
                UnitIntervalScalar64::new(0.79),
                UnitIntervalScalar64::new(0.79),
            ),
            UnitIntervalScalar64::MAX
        );
    }

    #[test]
    fn test_wrapping_add() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.75).wrapping_add(UnitIntervalScalar64::new(0.125)),
            UnitIntervalScalar64::new(0.75 + 0.125)
        );

        std::assert_eq!(
            UnitIntervalScalar64::new(0.75).wrapping_add(UnitIntervalScalar64::new(0.625)),
            UnitIntervalScalar64::new(0.75 + 0.625 - 1.0)
        );
    }

    #[test]
    fn test_wrapping_sub() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.625).wrapping_sub(UnitIntervalScalar64::new(0.25)),
            UnitIntervalScalar64::new(0.625 - 0.25)
        );

        std::assert_eq!(
            UnitIntervalScalar64::new(0.6875).wrapping_sub(UnitIntervalScalar64::new(0.9375)),
            UnitIntervalScalar64::new(0.6875 - 0.9375 + 1.0)
        );
    }

    #[test]
    fn test_saturating_add() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.34375).saturating_add(UnitIntervalScalar64::new(0.03125)),
            UnitIntervalScalar64::new(0.34375 + 0.03125)
        );

        std::assert_eq!(
            UnitIntervalScalar64::new(0.375).saturating_add(UnitIntervalScalar64::new(0.9375)),
            UnitIntervalScalar64::MAX
        );
    }

    #[test]
    fn test_saturating_sub() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.40625).saturating_sub(UnitIntervalScalar64::new(0.25)),
            UnitIntervalScalar64::new(0.40625 - 0.25)
        );

        std::assert_eq!(
            UnitIntervalScalar64::new(0.21875).saturating_sub(UnitIntervalScalar64::new(0.90625)),
            UnitIntervalScalar64::MIN
        );
    }

    #[test]
    fn test_equal_within_tolerance() {
        std::assert!(UnitIntervalScalar64::equal_within_tolerance(
            UnitIntervalScalar64::from_numerator(60401),
            UnitIntervalScalar64::from_numerator(60404),
            3
        ));

        std::assert!(UnitIntervalScalar64::equal_within_tolerance(
            UnitIntervalScalar64::from_numerator(25826),
            UnitIntervalScalar64::from_numerator(25824),
            2
        ));

        std::assert!(!UnitIntervalScalar64::equal_within_tolerance(
            UnitIntervalScalar64::from_numerator(58469),
            UnitIntervalScalar64::from_numerator(58479),
            4
        ));

        std::assert!(!UnitIntervalScalar64::equal_within_tolerance(
            UnitIntervalScalar64::from_numerator(77247),
            UnitIntervalScalar64::from_numerator(77241),
            4
        ));
    }

    #[test]
    fn test_add() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.75) + UnitIntervalScalar64::new(0.125),
            UnitIntervalScalar64::new(0.75 + 0.125)
        );
    }

    #[test]
    fn test_sub() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.625) - UnitIntervalScalar64::new(0.25),
            UnitIntervalScalar64::new(0.625 - 0.25)
        );
    }

    #[test]
    fn test_mul() {
        std::assert_eq!(
            UnitIntervalScalar64::new(0.875) * UnitIntervalScalar64::new(0.375),
            UnitIntervalScalar64::new(0.875 * 0.375)
        );
    }
}
