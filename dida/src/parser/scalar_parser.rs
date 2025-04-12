use crate::ScalarDeg1;
use crate::parser::parser::Parser;

fn is_digit(utf8_byte: u8) -> bool {
    utf8_byte >= b'0' && utf8_byte <= b'9' 
}

fn next_is_digit(parser: &Parser) -> bool {
    parser.next_is(&is_digit)
}

fn skip_digits(parser: &mut Parser) {
    parser.skip_zero_or_more(&is_digit)
}

fn try_parse_digit(parser: &mut Parser) -> Option<i32> {
    parser.try_parse_byte(&is_digit).map(|utf8_byte| (utf8_byte - b'0') as i32)
}

/// Parses a 'ScalarDeg1'.
pub fn parse_scalar_deg1(parser: &mut Parser) -> Option<ScalarDeg1> {
    const MAX_INT_PART: i32 = -(i32::MIN / ScalarDeg1::DENOM);

    let negative = parser.try_match(b'-');

    let has_integer_digits = next_is_digit(&parser);
    let mut int_part: i32 = 0;
    while let Some(digit) = try_parse_digit(parser) {
        if int_part >= MAX_INT_PART / 10 {
            if int_part > MAX_INT_PART / 10 || digit > MAX_INT_PART % 10 {
                skip_digits(parser);
                return None
            } 
        }

        int_part = 10 * int_part + digit;
    }

    let fractional_part: ScalarDeg1;
    if parser.try_match(b'.') && next_is_digit(&parser) {
        fractional_part = parse_scalar_fractional_part(parser);
    } else {
        if !has_integer_digits {
            return None;
        }

        fractional_part = ScalarDeg1::new(0.0);
    }

    if negative {
        std::debug_assert!(int_part <= MAX_INT_PART);

        let int_part_scalar = ScalarDeg1::from_numerator(-int_part * ScalarDeg1::DENOM);
        if -fractional_part < ScalarDeg1::MIN - int_part_scalar {
            return None;
        }

        return Some(int_part_scalar - fractional_part);
    } else {
        if int_part >= MAX_INT_PART {
            return None;
        }

        let int_part_scalar = ScalarDeg1::from_numerator(int_part * ScalarDeg1::DENOM);
        if fractional_part > ScalarDeg1::MAX - int_part_scalar {
            return None;
        }

        return Some(int_part_scalar + fractional_part);
    }
}

/// Returns the number of significant fractional digits in the base 10 representation of a scalar of the given type.
///
/// This is the number of digits such that truncating the decimal representation to this many digits results in a value
/// which is less than half a quantum lower than the untruncated value:
///
///   v - truncated(v) < .5 * ScalarType::quantum
///
const fn base_10_num_significant_fractional_digits() -> usize {
    // The result is the lowest 'n' for which
    //
    //   1/10^n <= .5 * 1/2^exp
    //
    // Taking the reciprocal gives
    //
    //   10^n >= 2^exp

    let mut lhs: i32 = 1;
    let mut n: usize = 0;
    while lhs < ScalarDeg1::DENOM {
        lhs *= 10;
        n += 1;
    }

    n
}

/// Parses the digits after the decimal point as a ScalarDeg1 in the range [0, 1).
fn parse_scalar_fractional_part(parser: &mut Parser) -> ScalarDeg1 {
    const NUM_SIGNIFICANT_DIGITS: usize = base_10_num_significant_fractional_digits();

    let mut base_10_num: i32 = 0;
    let mut base_10_denom: i32 = 1;
    for _ in 0..NUM_SIGNIFICANT_DIGITS {
        match try_parse_digit(parser) {
            Some(digit) => {
                base_10_num = 10 * base_10_num + digit;
                base_10_denom *= 10;
            },
            None => break,
        }
    }

    // The significant digits have been parsed. The final value will be either base_2_num / base_2_denom or
    // (base_2_num + 1) / base_2_denom.
    let mut base_2_num = base_10_num * ScalarDeg1::DENOM / base_10_denom;
    let remainder = base_10_num * ScalarDeg1::DENOM % base_10_denom;

    if remainder > (base_10_denom / 2) || (remainder == (base_10_denom / 2) && (base_2_num & 1) == 1) {
        // We're already rounding up, so even if there are digits remaining, these can't be enough to bump the
        // result up by another ScalarDeg1::QUANTUM.

        // Skip remaining digits.
        skip_digits(parser);
        return ScalarDeg1::from_numerator(base_2_num + 1);
    }

    if !next_is_digit(parser) {
        return ScalarDeg1::from_numerator(base_2_num);
    }

    // The truncated value resulted in downwards rounding, but there are digits remaining, so it is  possible that
    // these remaining digits push 'v' over the threshold for upwards rounding.
    //
    // We should round up if
    //
    //                     truncated(v) + tail(v) > base_2_num / base_2_denom + .5 / base_2_denom
    //                                    tail(v) > .5 / base_2_denom - remainder / (base_10_denom * base_2_denom)
    //   (tail(v) * base_10_denom) * base_2_denom > base_10_denom / 2 - remainder
    //
    let mut threshold = base_10_denom / 2 - remainder;

    while let Some(digit) = try_parse_digit(parser) {
        let digit_base_2_num = digit * ScalarDeg1::DENOM;
        threshold *= 10;
        if digit_base_2_num + ScalarDeg1::DENOM <= threshold {
            skip_digits(parser);
            return ScalarDeg1::from_numerator(base_2_num);
        } else if digit_base_2_num > threshold {
            skip_digits(parser);
            return ScalarDeg1::from_numerator(base_2_num + 1);
        } else {
            threshold -= digit_base_2_num;
        }
    }

    if threshold == 0 {
        // We're exactly on the threshold, so we should round to even.
        if (base_2_num & 1) == 1 {
            base_2_num += 1;
        }
    }

    ScalarDeg1::from_numerator(base_2_num)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_is_digit() {
        std::assert!(is_digit(b'0'));
        std::assert!(is_digit(b'5'));
        std::assert!(is_digit(b'9'));
        std::assert!(!is_digit(b'a'));
    }

    #[test]
    fn test_next_is_digit() {

    }
    
    /*fn next_is_digit(parser: &Parser) -> bool {
        parser.next_is(&is_digit)
    }
    
    fn skip_digits(parser: &mut Parser) {*/

    #[test]
    fn test_try_parse_digit() {
        {
            let mut parser = Parser::new("123");
            std::assert_eq!(try_parse_digit(&mut parser), Some(1));
            std::assert_eq!(parser.remainder(), "23");
        }

        {
            let mut parser = Parser::new("b");
            std::assert_eq!(try_parse_digit(&mut parser), None);
            std::assert_eq!(parser.remainder(), "b");
        }

        {
            let mut parser = Parser::new("");
            std::assert_eq!(try_parse_digit(&mut parser), None);
            std::assert_eq!(parser.remainder(), "");
        }
    }

    #[test]
    fn test_parse_scalar_deg1() {
        // Positive integer, ends at end of string.
        {
            let mut parser = Parser::new("3495");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(3495.0)));
            std::assert!(parser.has_finished());
        }

        // Positive integer, ends at other character.
        {
            let mut parser = Parser::new("3495, ");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(3495.0)));
            std::assert_eq!(parser.remainder(), ", ");
        }

        // Largest positive integer.
        {
            let mut parser = Parser::new("524287");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(524287.0)));
            std::assert!(parser.has_finished());
        }

        // Positive integer, too large.
        {
            let mut parser = Parser::new("524288");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // Positive integer, way too large.
        {
            let mut parser = Parser::new("1000524287");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // Negative integer, ends at end of string.
        {
            let mut parser = Parser::new("-2421");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-2421.0)));
            std::assert!(parser.has_finished());
        }

        // Negative integer, ends at other character.
        {
            let mut parser = Parser::new("-2421, ");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-2421.0)));
            std::assert_eq!(parser.remainder(), ", ");
        }

        // Largest negative integer.
        {
            let mut parser = Parser::new("-524288");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-524288.0)));
            std::assert!(parser.has_finished());
        }

        // Negative integer, too large.
        {
            let mut parser = Parser::new("-524289");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // Negative integer, way too large.
        {
            let mut parser = Parser::new("-1000524288");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // Positive with fractional part, ends at end of string.
        {
            let mut parser = Parser::new("7604.375");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(7604.375)));
            std::assert!(parser.has_finished());
        }

        // Positive with fractional part, ends at other character.
        {
            let mut parser = Parser::new("7604.375, ");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(7604.375)));
            std::assert_eq!(parser.remainder(), ", ");
        }

        // Largest positive value.
        {
            let mut parser = Parser::new("524287.999755859375");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::MAX));
            std::assert!(parser.has_finished());
        }

        // Positive, overflow in fractional part.
        {
            let mut parser = Parser::new(".123");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(0.123)));
            std::assert!(parser.has_finished());
        }

        // Positive, fractional part only, no leading zero.
        {
            let mut parser = Parser::new("524287.99990234374");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // Negative with fractional part, ends at end of string.
        {
            let mut parser = Parser::new("-7412.114");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-7412.114)));
            std::assert!(parser.has_finished());
        }

        // Negative with fractional part, ends at other character.
        {
            let mut parser = Parser::new("-7412.114, ");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-7412.114)));
            std::assert_eq!(parser.remainder(), ", ");
        }

        // Negative, rounds to lowest value.
        {
            let mut parser = Parser::new("-524288.000001");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::MIN));
            std::assert!(parser.has_finished());
        }

        // Negative, overflow in fractional part.
        {
            let mut parser = Parser::new("-524288.01");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        //  Negative, fractional part only, no leading zero.
        {
            let mut parser = Parser::new("-.123");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-0.123)));
            std::assert!(parser.has_finished());
        }

        // Ends in dot but no fractional part, positive.
        {
            let mut parser = Parser::new("6330.");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(6330.0)));
            std::assert!(parser.has_finished());
        }

        // Ends in dot but no fractional part, negative.
        {
            let mut parser = Parser::new("-4998.");
            std::assert_eq!(parse_scalar_deg1(&mut parser), Some(ScalarDeg1::new(-4998.0)));
            std::assert!(parser.has_finished());
        }

        // . is invalid.
        {
            let mut parser = Parser::new(".");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // - is invalid.
        {
            let mut parser = Parser::new("-");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // -. is invalid.
        {
            let mut parser = Parser::new("-");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert!(parser.has_finished());
        }

        // Empty string
        std::assert_eq!(parse_scalar_deg1(&mut Parser::new("")), None);

        // Unparsable characters.
        {
            let mut parser = Parser::new("not a number");
            std::assert_eq!(parse_scalar_deg1(&mut parser), None);
            std::assert_eq!(parser.remainder(), "not a number");
        }
    }

    #[test]
    fn test_parse_scalar_fractional_part() {
        fn parse(string: &str) -> ScalarDeg1 {
            let mut parser = Parser::new(string);
            let result = parse_scalar_fractional_part(&mut parser);
            std::assert!(parser.has_finished());
            result
        }

        // Few significant digits, exact.
        std::assert_eq!(parse("25"), ScalarDeg1::new(0.25));

        // Few significant digits, round up.
        std::assert_eq!(parse("123"), ScalarDeg1::new(0.123047));

        // Few significant digits, round down.
        std::assert_eq!(parse("126"), ScalarDeg1::new(0.125977));

        // Ignore tail if significant digits round up.
        std::assert_eq!(parse("23659891"), ScalarDeg1::new(0.236572));
            
        // First tail digit lower than threshold digit.
        std::assert_eq!(parse("3148099"), ScalarDeg1::new(0.314697));

        // First tail digit greater than threshold digit.
        std::assert_eq!(parse("31482001"), ScalarDeg1::new(0.314941));

        // Tail digit lower than threshold digit.
        std::assert_eq!(parse("31481932"), ScalarDeg1::new(0.314697));

        // Tail digit greater than threshold digit.
        std::assert_eq!(parse("31481934"), ScalarDeg1::new(0.314941));

        // Tail matches threshold, round down.
        std::assert_eq!(parse("8345947265625"), ScalarDeg1::new(0.834473));

        // Tail matches threshold, round up.
        std::assert_eq!(parse("5081787109375"), ScalarDeg1::new(0.508301));

        // Tail too short.
        std::assert_eq!(parse("508178710"), ScalarDeg1::new(0.508057));
    }
}