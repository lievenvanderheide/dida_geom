use crate::parser::parser::Parser;
use crate::parser::scalar_parser::parse_scalar_deg1;
use crate::{ScalarDeg1, Vec2, Point2};

/// Parses a 'Vec2'.
pub fn parse_vec2(parser: &mut Parser) -> Option<Vec2> {
    let Some(elems) = parser.parse_fixed_size_list::<ScalarDeg1, 2>(&parse_scalar_deg1) else {
        return None;
    };

    Some(Vec2::from_coords(elems[0], elems[1]))
}

/// Parses a 'Point2'.
pub fn parse_point2(parser: &mut Parser) -> Option<Point2> {
    parse_vec2(parser).map(&Point2::from_vec2)
}

#[cfg(test)]
mod tests {
    use super::*;

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
}