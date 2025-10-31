use crate::bezier_64::unit_interval_scalar::UnitIntervalScalar;
use crate::parser::parser::Parser;
use crate::parser::geometry_parsers::parse_unit_interval_vector;
use std::ops::{Add, Mul, Sub};
use std::str::FromStr;

/// A vector with UnitIntervalScalar coordinates.
#[derive(Clone, Copy, Debug, PartialEq)]
pub struct UnitIntervalVector<const DIM: usize> {
    coords: [UnitIntervalScalar; DIM]
}

impl<const DIM: usize> UnitIntervalVector<DIM> {
    /// Constructs a `UnitIntervalVector` with the given coordinates.
    pub fn new(coords: [f64; DIM]) -> Self {
        Self {
            coords: std::array::from_fn(|i| UnitIntervalScalar::new(coords[i])),
        }
    }

    /// Constructs a `UnitIntervalVector` with the given coordinates.
    pub fn from_coords(coords: [UnitIntervalScalar; DIM]) -> Self {
        Self { coords }
    }

    /// Returns a reference to the array of coordinates of this vector. 
    pub fn coords(&self) -> &[UnitIntervalScalar; DIM] {
        &self.coords
    }

    /// Returns the mid point between vector `a` and `b`.
    pub fn midpoint(&self, b: &Self) -> Self {
        Self {
            coords: std::array::from_fn(|i| self.coords[i].midpoint(b.coords[i])),
        }
    }

    /// Interpolates between vector `a` an `b` using interpolation parameter `t` and returns the result.
    pub fn interpolate(a: &Self, b: &Self, t: UnitIntervalScalar) -> Self {
        Self {
            coords: std::array::from_fn(|i| UnitIntervalScalar::interpolate(a.coords[i], b.coords[i], t)),
        }
    }

    /// Returns the component wise minimum of the given two vectors.
    pub fn component_wise_min(&self, b: &Self) -> Self {
        Self {
            coords: std::array::from_fn(|i| std::cmp::min(self.coords[i], b.coords[i])),
        }
    }

    /// Returns the component wise maximum of the given two vectors.
    pub fn component_wise_max(&self, b: &Self) -> Self {
        Self {
            coords: std::array::from_fn(|i| std::cmp::max(self.coords[i], b.coords[i])),
        }
    }

    /// Compares two vectors with a tolerance. See `UnitIntervalScalar::equal_with_tolerance` for more details.
    pub fn equal_within_tolerance(&self, b: &Self, error_number: usize) -> bool {
        for i in 0..DIM {
            if !UnitIntervalScalar::equal_within_tolerance(self.coords[i], b.coords[i], error_number) {
                return false;
            }
        }

        true
    }
}

impl<const DIM: usize> Default for UnitIntervalVector<DIM> {
    fn default() -> Self {
        Self {
            coords: [UnitIntervalScalar::default(); DIM],
        }
    }
}

impl<const DIM: usize> Add for UnitIntervalVector<DIM> {
    type Output = Self;

    fn add(self, b: Self) -> Self {
        Self {
            coords: std::array::from_fn(|i| self.coords[i] + b.coords[i])
        }
    }
}

impl<const DIM: usize> Sub for UnitIntervalVector<DIM> {
    type Output = Self;

    fn sub(self, b: Self) -> Self {
        Self {
            coords: std::array::from_fn(|i| self.coords[i] - b.coords[i])
        }
    }
}

impl<const DIM: usize> Mul<UnitIntervalScalar> for UnitIntervalVector<DIM> {
    type Output = Self;

    fn mul(self, b: UnitIntervalScalar) -> Self {
        Self {
            coords: std::array::from_fn(|i| self.coords[i] * b)
        }
    }
}

impl<const DIM: usize> FromStr for UnitIntervalVector<DIM> {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, String> {
        let mut parser = Parser::new(s);

        parser.skip_optional_whitespace();
        let Some(result) = parse_unit_interval_vector(&mut parser) else {
            return Err(format!("Failed to parse UnitIntervalScalar \"{}\"", s));
        };

        parser.skip_optional_whitespace();
        if !parser.has_finished() {
            return Err(format!("Failed to parse UnitIntervalScalar \"{}\"", s));
        }

        Ok(result)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_new_and_access_coords() {
        let vector = UnitIntervalVector::new([0.0074, 0.5135]);
        std::assert_eq!(vector.coords(), &[UnitIntervalScalar::new(0.0074), UnitIntervalScalar::new(0.5135)]);
    }

    #[test]
    fn test_from_coords_and_acces() {
        let coords = [
            UnitIntervalScalar::new(0.475),
            UnitIntervalScalar::new(0.243),
            UnitIntervalScalar::new(0.071),
            UnitIntervalScalar::new(0.029),
        ];

        let vector = UnitIntervalVector::from_coords(coords);
        std::assert_eq!(vector.coords(), &coords);
    }

    #[test]
    fn test_midpoint() {
        let a = UnitIntervalVector::new([0.15625, 0.34375, 0.5]);
        let b = UnitIntervalVector::new([0.125, 0.40625, 0.3125]);
        let midpoint = a.midpoint(&b);
        std::assert_eq!(midpoint, UnitIntervalVector::new([0.140625, 0.375, 0.40625]));
    }

    #[test]
    fn test_interpolate() {
        let a = UnitIntervalVector::new([0.25,  0.9375]);
        let b = UnitIntervalVector::new([0.65625, 0.5625]);
        let t = UnitIntervalScalar::new(0.4375);
        std::assert_eq!(
            UnitIntervalVector::interpolate(&a, &b, t),
            UnitIntervalVector::new([0.427734375, 0.7734375])
        );
    }

    #[test]
    fn test_component_wise_min() {
        let a = UnitIntervalVector::new([0.189, 0.614, 0.387]);
        let b = UnitIntervalVector::new([0.279, 0.311, 0.513]);
        std::assert_eq!(
            a.component_wise_min(&b),
            UnitIntervalVector::new([0.189, 0.311, 0.387])
        );
    }

    #[test]
    fn test_component_wise_max() {
        let a = UnitIntervalVector::new([0.114, 0.356, 0.859, 0.973]);
        let b = UnitIntervalVector::new([0.656, 0.460, 0.002, 0.217]);
        std::assert_eq!(
            a.component_wise_max(&b),
            UnitIntervalVector::new([0.656, 0.460, 0.859, 0.973])
        );
    }

    #[test]
    fn test_add() {
        let a = UnitIntervalVector::new([0.5, 0.375, 0.25]);
        let b = UnitIntervalVector::new([0.125, 0.375, 0.125]);
        std::assert_eq!(
            a + b,
            UnitIntervalVector::new([0.625, 0.75, 0.375])
        );
    }

    #[test]
    fn test_sub() {
        let a = UnitIntervalVector::new([0.65625, 0.5625]);
        let b = UnitIntervalVector::new([0.625, 0.3125]);
        std::assert_eq!(
            a - b,
            UnitIntervalVector::new([0.03125, 0.25])
        );
    }

    #[test]
    fn test_mul() {
        let a = UnitIntervalVector::new([0.90625, 0.625, 0.59375]);
        let b = UnitIntervalScalar::new(0.15625);
        std::assert_eq!(
            a * b,
            UnitIntervalVector::new([0.1416015625, 0.09765625, 0.0927734375])
        );
    }

    #[test]
    fn test_from_str() {
        std::assert_eq!(
            UnitIntervalVector::from_str("{0.012, 0.796, 0.765}"),
            Ok(UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_str("0.012").unwrap(),
                UnitIntervalScalar::from_str("0.796").unwrap(),
                UnitIntervalScalar::from_str("0.765").unwrap(),
            ]))
        );

        std::assert_eq!(
            UnitIntervalVector::from_str("  {0.318, 0.097}    "),
            Ok(UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_str("0.318").unwrap(),
                UnitIntervalScalar::from_str("0.097").unwrap(),
            ]))
        );

        std::assert!(UnitIntervalVector::<2>::from_str("NotAVector").is_err());
    }

    #[test]
    fn test_equal_within_tolerance() {
        std::assert!(UnitIntervalVector::equal_within_tolerance(
            &UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_numerator(761243),
                UnitIntervalScalar::from_numerator(719168),
                UnitIntervalScalar::from_numerator(264298),
            ]),
            &UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_numerator(761246),
                UnitIntervalScalar::from_numerator(719164),
                UnitIntervalScalar::from_numerator(264295),
            ]),
            4
        ));

        std::assert!(!UnitIntervalVector::equal_within_tolerance(
            &UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_numerator(618388),
                UnitIntervalScalar::from_numerator(524106),
            ]),
            &UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_numerator(618388),
                UnitIntervalScalar::from_numerator(524116),
            ]),
            4
        ));
    }
}
