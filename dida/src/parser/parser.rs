use std::str;
use crate::container::array_builder::ArrayBuilder;

pub struct Parser<'a> {
    utf8_bytes: &'a [u8],
    byte_index: usize, 
}

impl<'a> Parser<'a> {
    /// Constructs a parser with the text to parse.
    pub fn new(string: &'a str) -> Self {
        Self {
            utf8_bytes: string.as_bytes(),
            byte_index: 0,
        }
    }
    
    /// Returns the remaining text to parse.
    #[cfg(test)]
    pub fn remainder(&self) -> &str {
        str::from_utf8(&self.utf8_bytes[self.byte_index..]).unwrap()
    }

    /// Returns whether the full text has been parsed.
    pub fn has_finished(&self) -> bool {
        self.byte_index == self.utf8_bytes.len()
    }

    /// Returns whether the next byte to parse matches the given predicate. If there's no text left to parse, then
    /// 'false' is returned.
    pub fn next_is(&self, pred: impl Fn(u8) -> bool) -> bool {
        self.byte_index < self.utf8_bytes.len() && pred(self.utf8_bytes[self.byte_index])
    }
    
    /// Checks whether the next byte to parse matches the given predicate, if so, advances the parser to the next byte
    /// and returns the byte which was parsed. If the next byte doesn't match the predicate, or if there's no text left
    /// to parse, then the return value is 'None' and the parser state is left unchanged.
    pub fn try_parse_byte(&mut self, pred: impl Fn(u8) -> bool) -> Option<u8> {
        if self.byte_index < self.utf8_bytes.len() && pred(self.utf8_bytes[self.byte_index]) {
            let byte_index = self.byte_index;
            self.byte_index += 1;
            Some(self.utf8_bytes[byte_index])
        } else {
            None
        }
    }

    /// Checks whether the next byte to parse is equal to the given 'utf8_byte', if so, advances the parser to the next
    /// byte. If the next byte doesn't match 'utf8_byte' or if the end of the text to parse has been reached, then the
    /// parser state is left unchanged.
    ///
    /// The return value is 'true' iff there was a match.
    pub fn try_match(&mut self, utf8_byte: u8) -> bool {
        if self.byte_index < self.utf8_bytes.len() && self.utf8_bytes[self.byte_index] == utf8_byte {
            self.byte_index += 1;
            true
        } else {
            false
        }
    }

    /// Skips bytes which match the predicate, until the first byte which doesn't match the predicate.
    pub fn skip_zero_or_more(&mut self, pred: impl Fn(u8) -> bool) {
        while self.next_is(&pred) {
            self.byte_index += 1;
        }
    }

    fn is_whitespace(c: u8) -> bool {
        c == b' ' || c == b'\t' || c == b'\n' || c == b'\r'
    }

    /// Skips whitespace, if any, until the first non-whitespace byte.
    pub fn skip_optional_whitespace(&mut self) {
        self.skip_zero_or_more(&Self::is_whitespace)
    }

    pub fn parse_fixed_size_list<T, const N: usize>(
        &mut self,
        parse_elem: &impl Fn(&mut Parser) -> Option<T>
    ) -> Option<[T; N]> {
        if !self.try_match(b'{') {
            return None;
        }

        let mut result_builder = ArrayBuilder::<T, N>::new();
        for i in 0..N {
            self.skip_optional_whitespace();
            let Some(elem) = parse_elem(self) else {
                return None;
            };

            result_builder.push(elem);

            if i != N - 1 {
                self.skip_optional_whitespace();
                if !self.try_match(b',') {
                    return None;
                }
            }
        }

        self.skip_optional_whitespace();
        if !self.try_match(b'}') {
            return None;
        }
    
        Some(result_builder.finalize())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_functions() {
        let mut parser = Parser::new("Dida");
        
        std::assert_eq!(parser.remainder(), "Dida");
        std::assert!(!parser.next_is(|c| c == b'i'));

        let mut expected_remainder = "Dida";
        for expected_c in [b'D', b'i', b'd', b'a'] {
            println!("{}", expected_c);

            std::assert!(!parser.has_finished());

            std::assert!(parser.next_is(|c| c == expected_c));
            std::assert_eq!(parser.try_parse_byte(|c| c == b'x'), None);
            std::assert!(parser.next_is(|c| c == expected_c));
            std::assert_eq!(parser.try_parse_byte(|c| c == expected_c), Some(expected_c));
            std::assert!(!parser.next_is(|c| c == expected_c));
        
            expected_remainder = &expected_remainder[1..];
            std::assert_eq!(parser.remainder(), expected_remainder);
        }

        std::assert!(parser.has_finished());
    }

    #[test]
    fn test_try_match() {
        let mut parser = Parser::new("Dida");

        std::assert!(!parser.try_match(b'd'));
        std::assert!(parser.try_match(b'D'));
        std::assert!(parser.try_match(b'i'));
        std::assert!(parser.try_match(b'd'));
        std::assert!(parser.try_match(b'a'));
        std::assert!(!parser.try_match(b'd'));
        std::assert!(parser.has_finished());
    }

    #[test]
    fn test_skip_zero_or_more() {
        let mut parser = Parser::new("xXxYyyYZZZzz");
        
        parser.skip_zero_or_more(|c| c == b'x' || c == b'X');
        std::assert_eq!(parser.remainder(), "YyyYZZZzz");

        parser.skip_zero_or_more(|c| c == b'z' || c == b'Z');
        std::assert_eq!(parser.remainder(), "YyyYZZZzz");
        
        parser.skip_zero_or_more(|c| c == b'y' || c == b'Y');
        std::assert_eq!(parser.remainder(), "ZZZzz");

        parser.skip_zero_or_more(|c| c == b'z' || c == b'Z');
        std::assert_eq!(parser.remainder(), "");
    }

    #[test]
    fn test_skip_optional_whitespace() {
        let mut parser = Parser::new("      a \t\n   b \r ");

        parser.skip_optional_whitespace();
        std::assert!(parser.try_match(b'a'));

        parser.skip_optional_whitespace();
        std::assert!(parser.try_match(b'b'));

        parser.skip_optional_whitespace();
        std::assert!(parser.has_finished());
    }
}