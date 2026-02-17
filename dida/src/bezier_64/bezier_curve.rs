use crate::bezier_64::unit_interval_scalar::UnitIntervalScalar;
use crate::bezier_64::unit_interval_vector::UnitIntervalVector;
use crate::parser::parser::Parser;
use crate::parser::geometry_parsers::parse_bezier_curve;
use std::str::FromStr;

/// A bezier curve with `UnitIntervalVector` control points.
#[derive(Clone, Debug, PartialEq)]
pub struct BezierCurve<const DIM: usize, const ORDER: usize> {
    control_points: [UnitIntervalVector<DIM>; ORDER],
}

impl<const DIM: usize, const ORDER: usize> BezierCurve<DIM, ORDER> {
    /// Constructs a new `BezierCurve` from `f64` coordinates.
    pub fn new(control_points_f64: [[f64; DIM]; ORDER]) -> Self {
        Self {
            control_points: std::array::from_fn(|i| UnitIntervalVector::new(control_points_f64[i])),
        }
    }

    /// Constructs a new `BezierCurve` with the given control points.
    pub fn from_control_points(control_points: [UnitIntervalVector<DIM>; ORDER]) -> Self {
        Self { control_points }
    }

    /// Returns the control points of this `BezierCurve`.
    pub fn control_points(&self) -> &[UnitIntervalVector<DIM>; ORDER] {
        &self.control_points
    }

    /// Evaluates the value of the curve at parameter `t`.
    ///
    /// The error number of each of the coordinates of the result is
    ///
    ///   error_number = (ORDER - 1) * (error_number(t) + 1) + error_number(self)
    ///
    /// Where error_number(self) is the max error number of corresponding coordinate of the control points.
    pub fn eval(&self, t: UnitIntervalScalar) -> UnitIntervalVector<DIM> {
        let mut control_points = self.control_points;
        for i in 1..ORDER {
            for j in 0..ORDER - i {
                control_points[j] = UnitIntervalVector::interpolate(&control_points[j], &control_points[j + 1], t);
            }
        }

        control_points[0]
    }

    /// Splits the bezier curve at the point with parameter 't'.
    ///
    /// The error number of each of the coordinates of the result is
    ///
    ///   error_number = (ORDER - 1) * (error_number(t) + 1) + error_number(self)
    ///
    /// Where error_number(self) is the max error number of corresponding coordinate of the control points.
    pub fn split_at(&self, t: UnitIntervalScalar) -> (Self, Self) {
        let mut left = BezierCurve::default();
        let mut right = self.clone();

        for i in 0..ORDER {
            left.control_points[i] = right.control_points[0];
            for j in 0..ORDER - i - 1 {
                right.control_points[j] = UnitIntervalVector::interpolate(
                    &right.control_points[j],
                    &right.control_points[j + 1],
                    t
                );
            }
        }

        (left, right)
    }

    /// Splits the bezier curve at the point with parameter 0.5.
    ///
    /// The error number of each of the coordinates in the result is
    ///
    ///   error_number = (ORDER - 1) + error_number(self)
    ///
    /// Where error_number(self) is the max error number of the coordinates in 'self'.
    pub fn split_at_mid(&self) -> (Self, Self) {
        let mut left = BezierCurve::default();
        let mut right = self.clone();

        for i in 0..ORDER {
            left.control_points[i] = right.control_points[0];
            for j in 0..ORDER - i - 1 {
                right.control_points[j] = UnitIntervalVector::midpoint(
                    &right.control_points[j],
                    &right.control_points[j + 1]
                );
            }
        }

        (left, right)
    }
}


impl<const DIM: usize, const ORDER: usize> Default for BezierCurve<DIM, ORDER> {
    fn default() -> Self {
        Self {
            control_points: [UnitIntervalVector::default(); ORDER],
        }
    }
}

impl<const DIM: usize, const ORDER: usize> FromStr for BezierCurve<DIM, ORDER> {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, String> {
        let mut parser = Parser::new(s);

        parser.skip_optional_whitespace();
        let Some(result) = parse_bezier_curve::<DIM, ORDER>(&mut parser) else {
            return Err(format!("Failed to parse BezierCurve<{}, {}> \"{}\"", DIM, ORDER, s));
        };

        parser.skip_optional_whitespace();
        if !parser.has_finished() {
            return Err(format!("Failed to parse BezierCurve<{}, {}> \"{}\"", DIM, ORDER, s));
        }

        Ok(result)
    }
}

struct BezierCurveSlice<const DIM: usize, const ORDER: usize> {
    /// The curve corresponding to this slice.
    ///
    /// The point on this curve with param 't' is equal to the point on the original curve with param
    /// 'min_t + t * (max_t - min_t)'.
    curve: BezierCurve<DIM, ORDER>,

    /// The param of the point on the original curve corresponding to the leftmost point of 'curve'.
    min_t: UnitIntervalScalar,

    /// The param of the point on the original curve corresponding to the rightmost point of 'curve'.
    max_t: UnitIntervalScalar,
}

impl<const DIM: usize, const ORDER: usize> BezierCurveSlice<DIM, ORDER> {
    pub fn full_curve_slice(curve: BezierCurve<DIM, ORDER>) -> Self {
        BezierCurveSlice {
            curve: curve,
            min_t: UnitIntervalScalar::MIN,
            max_t: UnitIntervalScalar::MAX,
        }
    }

    /// Splits the bezier slice at its midpoint.
    ///
    /// The error number of each of the coordinates in the result is
    ///
    ///   error_number = (ORDER - 1) + error_number(self)
    ///
    /// where error_number(self) is the max error number of the coordinates in 'self'.
    ///
    /// In general, the error number of the bounds of the param range is
    ///
    ///   error_number(min_t, max_t) = max(error_number(self.min_t), error_number(self.max_t)) + 1.
    ///
    /// However, if it's a slice which was arrived at by successive calls to `split_at_mid` while starting from the
    /// slice returned by `full_curve_slice`, then the error number is at most 1 (regardless of the number of times
    /// `split_at_mid` was called).
    ///
    /// Proof: The full interval, represented using numerators, is of the form [0, 2^n - 1]. Splitting this interval
    /// into 2 results in the intervals [0, 2^(n - 1) - 1] and [2^(n - 1) - 1, 2^n - 1]. The second interval has a width
    /// which is a power of 2, so all further recursive splits of it will be exact. The first interval is of the
    /// same form as the original interval, so we can apply induction. The conclusion is that the end points of each
    /// interval differ at most 1 from the exact endpoint.
    pub fn split_at_mid(&self) -> (Self, Self) {
        std::assert!(self.max_t - self.min_t >= UnitIntervalScalar::from_numerator(2));

        let (left_curve, right_curve) = self.curve.split_at_mid();
        let mid_t = self.min_t.midpoint(self.max_t);

        (
            Self {
                curve: left_curve,
                min_t: self.min_t,
                max_t: mid_t,
            },
            Self {
                curve: right_curve,
                min_t: mid_t,
                max_t: self.max_t,
            }
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new_and_access() {
        let curve = BezierCurve::new([
            [0.91, 0.18, 0.17],
            [0.54, 0.33, 0.03],
        ]);

        std::assert_eq!(curve.control_points(), &[
            UnitIntervalVector::new([0.91, 0.18, 0.17]),
            UnitIntervalVector::new([0.54, 0.33, 0.03]),
        ]);
    }

    #[test]
    fn test_from_control_points_and_access() {
        let control_points = [
            UnitIntervalVector::<2>::from_str("{0.790, 0.562}").unwrap(),
            UnitIntervalVector::<2>::from_str("{0.234, 0.601}").unwrap(),
            UnitIntervalVector::<2>::from_str("{0.332, 0.034}").unwrap(),
        ];

        let curve = BezierCurve::from_control_points(control_points);
        std::assert_eq!(curve.control_points(), &control_points);
    }

    #[test]
    fn test_eval() {
        let curve = BezierCurve::<2, 3>::from_str("{{0.59, 0.65}, {0.81, 0.86}, {0.92, 0.83}}").unwrap();
        std::assert!(UnitIntervalVector::equal_within_tolerance(
            &curve.eval(UnitIntervalScalar::from_str("0.52").unwrap()),
            &UnitIntervalVector::from_str("{0.789056, 0.803504}").unwrap(),
            5
        ));
    }

    #[test]
    fn test_split_at() {
        let curve = BezierCurve::<2, 5>::from_str("
            {{0.231, 0.504}, {0.331, 0.685}, {0.023, 0.494}, {0.221, 0.590}, {0.566, 0.371}}
        ").unwrap();

        let t = UnitIntervalScalar::from_str("0.85").unwrap();
        let (left, right) = curve.split_at(t);

        let eval_t = UnitIntervalScalar::from_str("0.271").unwrap();
        std::assert!(UnitIntervalVector::equal_within_tolerance(
            &curve.eval(eval_t * t),
            &left.eval(eval_t),
            30
        ));

        let eval_t = UnitIntervalScalar::from_str("0.264").unwrap();
        std::assert!(UnitIntervalVector::equal_within_tolerance(
            &curve.eval(t + eval_t * UnitIntervalScalar::new(0.0).wrapping_sub(t)),
            &right.eval(eval_t),
            30
        ));
    }

    #[test]
    fn test_split_at_mid() {
        let curve = BezierCurve::<1, 4>::from_str("{{0.72}, {0.98}, {0.33}, {0.92}}").unwrap();
        let (left, right) = curve.split_at_mid();

        std::assert!(UnitIntervalVector::equal_within_tolerance(
            &curve.eval(UnitIntervalScalar::new(0.25)),
            &left.eval(UnitIntervalScalar::new(0.5)),
            3
        ));

        std::assert!(UnitIntervalVector::equal_within_tolerance(
            &curve.eval(UnitIntervalScalar::new(0.875)),
            &right.eval(UnitIntervalScalar::new(0.75)),
            3
        ));
    }

    #[test]
    fn test_from_str() {
        let curve = BezierCurve::<3, 4>::from_str(
            "{{0.988, 0.606, 0.541}, {0.215, 0.221, 0.86}, {0.701, 0.196, 0.070}, {0.599, 0.644, 0.207}}"
        ).unwrap();

        std::assert_eq!(
            curve.control_points(),
            &[
                UnitIntervalVector::from_str("{0.988, 0.606, 0.541}").unwrap(),
                UnitIntervalVector::from_str("{0.215, 0.221, 0.86}").unwrap(),
                UnitIntervalVector::from_str("{0.701, 0.196, 0.070}").unwrap(),
                UnitIntervalVector::from_str("{0.599, 0.644, 0.207}").unwrap(),
            ]
        )
    }

    #[test]
    fn test_beziez_curve_slice_full_curve_slice() {
        let curve = BezierCurve::<2, 3>::from_str("{{0.15, 0.18}, {0.66, 0.80}, {0.90, 0.18}}").unwrap();
        let slice = BezierCurveSlice::full_curve_slice(curve.clone());
        std::assert_eq!(slice.curve, curve);
        std::assert_eq!(slice.min_t, UnitIntervalScalar::MIN);
        std::assert_eq!(slice.max_t, UnitIntervalScalar::MAX);
    }

    #[test]
    fn test_bezier_curve_slice_split_at_mid() {
        let curve = BezierCurve::<1, 4>::from_str("{{0.1}, {0.9}, {0.3}, {0.6}}").unwrap();
        let slice_0_1 = BezierCurveSlice::full_curve_slice(curve.clone());
        let (slice_0_2, slice_1_2) = slice_0_1.split_at_mid();
        let (slice_0_4, slice_1_4) = slice_0_2.split_at_mid();
        let (slice_2_4, slice_3_4) = slice_1_2.split_at_mid();

        for slice in [slice_0_4, slice_1_4, slice_2_4, slice_3_4] {
            for slice_t in [UnitIntervalScalar::new(0.0), UnitIntervalScalar::new(0.4), UnitIntervalScalar::new(0.9)] {
                let value = slice.curve.eval(slice_t);
                let ref_value = curve.eval(slice.min_t + slice_t * (slice.max_t - slice.min_t));
                std::assert!(value.equal_within_tolerance(&ref_value, 10));
            }
        }
    }
}
