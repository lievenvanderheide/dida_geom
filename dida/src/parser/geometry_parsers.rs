use crate::parser::parser::Parser;
use crate::parser::scalar_parser::{parse_scalar_deg1, parse_unit_interval_scalar};
use crate::{ScalarDeg1, Vec2, Point2};
use crate::bezier_64::unit_interval_scalar::UnitIntervalScalar;
use crate::bezier_64::unit_interval_vector::UnitIntervalVector;
use crate::bezier_64::bezier_curve::BezierCurve;

/// Parses a 'Vec2'.
pub fn parse_vec2(parser: &mut Parser) -> Option<Vec2> {
    let Some(elems) = parser.parse_array::<ScalarDeg1, 2>(&parse_scalar_deg1) else {
        return None;
    };

    Some(Vec2::from_coords(elems[0], elems[1]))
}

/// Parses a 'Point2'.
pub fn parse_point2(parser: &mut Parser) -> Option<Point2> {
    parse_vec2(parser).map(&Point2::from_vec2)
}

/// Parses a vector of 'Point2' elements.
pub fn parse_point2_vec(parser: &mut Parser) -> Option<Vec<Point2>> {
    parser.parse_vector(&parse_point2)
}

/// Parses a `UnitIntervaVector`.
pub fn parse_unit_interval_vector<const DIM: usize>(parser: &mut Parser) -> Option<UnitIntervalVector<DIM>> {
    let Some(coords) = parser.parse_array::<UnitIntervalScalar, DIM>(&parse_unit_interval_scalar) else {
        return None;
    };

    Some(UnitIntervalVector::from_coords(coords))
}

/// Parses a `BezierCurve`.
pub fn parse_bezier_curve<const DIM: usize, const ORDER: usize>(
    parser: &mut Parser
) -> Option<BezierCurve<DIM, ORDER>> {
    let Some(control_points) = parser.parse_array::<UnitIntervalVector<DIM>, ORDER>(&parse_unit_interval_vector) else {
        return None;
    };

    Some(BezierCurve::from_control_points(control_points))
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::str::FromStr;

    #[test]
    fn test_parse_vec2() {
        std::assert_eq!(parse_vec2(&mut Parser::new("{12,34}")), Some(Vec2::new(12.0, 34.0)));
        std::assert_eq!(parse_vec2(&mut Parser::new("{  12   ,\n\t34  }")), Some(Vec2::new(12.0, 34.0)));
        std::assert_eq!(parse_vec2(&mut Parser::new("{ 95.8, -61.2 }")), Some(Vec2::new(95.8, -61.2)));
    
        std::assert_eq!(parse_vec2(&mut Parser::new("12, 34}")), None);
        std::assert_eq!(parse_vec2(&mut Parser::new("{x, 34}")), None);
        std::assert_eq!(parse_vec2(&mut Parser::new("{12; 34}")), None);
        std::assert_eq!(parse_vec2(&mut Parser::new("{12, y}")), None);
        std::assert_eq!(parse_vec2(&mut Parser::new("{12, 34")), None);
    }

    #[test]
    fn test_parse_point2() {
        std::assert_eq!(parse_point2(&mut Parser::new("{-2.41, 2.26}")), Some(Point2::new(-2.41, 2.26)));
        std::assert_eq!(parse_point2(&mut Parser::new("What's your point?")), None);
    }

    #[test]
    fn test_parse_point2_vec() {
        std::assert_eq!(
            parse_point2_vec(&mut Parser::new("{{-0.49, -6.08}, {2.84, 9.15}}")),
            Some(vec![Point2::new(-0.49, -6.08), Point2::new(2.84, 9.15)])
        );

        std::assert_eq!(parse_point2_vec(&mut Parser::new("{{-0.49, -6.08}, {cos(theta), sin(theta)}}")), None);
    }

    #[test]
    fn test_parse_unit_interval_vector() {
        std::assert_eq!(
            parse_unit_interval_vector(&mut Parser::new("{0.5, 0.7}")),
            Some(UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_str("0.5").unwrap(),
                UnitIntervalScalar::from_str("0.7").unwrap()
            ]))
        );

        std::assert_eq!(
            parse_unit_interval_vector(&mut Parser::new("{0.822, 0.17, 0.582, 0.429}")),
            Some(UnitIntervalVector::from_coords([
                UnitIntervalScalar::from_str("0.822").unwrap(),
                UnitIntervalScalar::from_str("0.17").unwrap(),
                UnitIntervalScalar::from_str("0.582").unwrap(),
                UnitIntervalScalar::from_str("0.429").unwrap(),
            ]))
        );

        std::assert_eq!(
            parse_unit_interval_vector::<5>(&mut Parser::new("{0.5, 0.7, 0.8}")),
            None
        );
    }

    #[test]
    fn test_parse_bezier_curve() {
        std::assert_eq!(
            parse_bezier_curve::<2, 3>(&mut Parser::new("{{0.268, 0.238}, {0.501, 0.282}, {0.678, 0.457}}")),
            Some(BezierCurve::from_control_points([
                UnitIntervalVector::from_str("{0.268, 0.238}").unwrap(),
                UnitIntervalVector::from_str("{0.501, 0.282}").unwrap(),
                UnitIntervalVector::from_str("{0.678, 0.457}").unwrap(),
            ]))
        );

        std::assert_eq!(
            parse_bezier_curve::<3, 4>(&mut Parser::new("{0.268, 0.238, 0.501}")),
            None
        );
    }
}
