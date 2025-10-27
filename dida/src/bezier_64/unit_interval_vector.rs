use crate::bezier_64::unit_interval_scalar::UnitIntervalScalar;
use std::ops::{Add, Mul, Sub};

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

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_new_and_access_coords() {
        let vector = UnitIntervalVector::new([0.0074, 0.5135]);
        std::assert_eq!(vector.coords, [UnitIntervalScalar::new(0.0074), UnitIntervalScalar::new(0.5135)]);
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
}
