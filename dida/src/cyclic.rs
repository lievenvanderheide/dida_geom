/// Returns the predecessor of `i` modulo `m`.
///
/// The input `i` must be normalized, that is, it must be less than `m`.
pub fn pred_modulo(i: usize, m: usize) -> usize {
    std::debug_assert!(i < m);
    if i == 0 { 
        m - 1
    } else {
        i - 1
    }
}

/// Returns the predecessor of `i` modulo `m`.
///
/// The input `i` must be normalized, that is, it must be less than `m`.
pub fn succ_modulo(mut i: usize, m: usize) -> usize {
    i += 1;
    if i == m { 
        0
    } else {
        i
    }
}

/// Returns the sum of `a` and `b` modulo `m`.
///
/// The following preconditions must be satified: `a < m` and `b <= m`.
pub fn add_modulo(mut a: usize, b: usize, m: usize) -> usize {
    std::debug_assert!(a < m);
    std::debug_assert!(b <= m);

    a += b;
    if a < m {
        a
    } else {
        a - m
    }
}

/// Returns the difference of `a` and `b` modulo `m`.
///
/// The following preconditions must be satified: `a < m` and `b <= m`.
pub fn sub_modulo(a: usize, b: usize, m: usize) -> usize {
    std::debug_assert!(a < m);
    std::debug_assert!(b <= m);
  
    if a >= b {
        a - b
    } else {
        m + a - b
    }
}

/// Returns the index of the maximum in a unimodal cyclic sequence. 
///
/// A unimodal cyclic sequence is a cyclic sequence for which the elements between the minimum and maximum are ascending
/// and the elements between the maximum and minimum descending.
///
/// The minimum and maximum elements may be repeated, but the remaining parts of the two subsequences must be strictly
/// ascending/descending.
pub fn unimodal_cyclic_sequence_maximum<T>(slice: &[T], less_than: impl Fn(&T, &T) -> bool) -> usize {
    std::assert!(slice.len() >= 2);

    let mut range_begin = 0;
    let mut range_end = slice.len();
    let mut range_begin_descending = less_than(&slice[range_begin], &slice[range_end - 1]);
    
    while range_begin + 1 < range_end {
        let range_mid = (range_begin + range_end) / 2;

        if less_than(&slice[range_mid], &slice[range_begin]) {
            if range_begin_descending {
                range_begin = range_mid;
                range_begin_descending = less_than(&slice[range_mid], &slice[range_mid - 1]);
            } else {
                range_end = range_mid;
            }
        } else {
            if less_than(&slice[range_mid], &slice[range_mid - 1]) {
                range_end = range_mid;
            } else {
                range_begin = range_mid;
                range_begin_descending = false;
                
            }
        }
    }

    range_begin
}

/// A type containing brackets within a unimodal sequence which contain a given value. A bracket for a value is a
/// subrange of the sequence which crosses the value exactly once.
#[derive(PartialEq, Debug)]
pub enum UnimodalCyclicSequenceBrackets {
    /// The general variant.
    ///
    /// If 'first_bracket_towards_right' is true, then the following holds:
    ///
    ///  - slice[i] <= value < slice[i + 1] for some i in 'begin_index <= i < mid_index'
    ///  - slice[(j + 1) % slice.len()] <= value < slice[j] for some j in 'mid_index <= j < end_index'
    ///
    /// If 'first_bracket_towards_right' is false, then the inequalities are flipped.
    Brackets {
        first_bracket_towards_right: bool,
        begin_index: usize,
        mid_index: usize,
        end_index: usize,
    },
    /// The variant for one of the following 2 special cases:
    ///
    ///  - If towards_right is true, then slice[0] < value and max(slice) == value.
    ///  - If towards_left is false, then slice[0] > value and min(slice) == value.
    ///
    /// 'index' is the index of the min/max element. If there are multiply such elements, then it will be the index of
    /// the last one. 
    ValueIsExtremalElement {
        towards_right: bool,
        index: usize,
    },
    /// Indicates the value is strictly outside the range of the sequence. 
    ValueOutsideRange,
}

impl UnimodalCyclicSequenceBrackets {
    pub fn brackets_for_value<T>(slice: &[T], value: &T, less_than: impl Fn(&T, &T) -> bool) -> Self {
        std::assert!(slice.len() >= 2);

        let first_bracket_towards_right = 'first_bracket_towards_right: {
            if less_than(&slice[0], value) {
                true
            } else if less_than(value, &slice[0]) {
                false
            } else {
                for i in 1..slice.len() {
                    if less_than(value, &slice[i]) {
                        break 'first_bracket_towards_right true;
                    } else if less_than(&slice[i], value) {
                        break 'first_bracket_towards_right false;
                    }
                }

                std::panic!(
                    "'slice' must contain at least one element which compares either less than or greater than 'value'"
                );
            }
        };

        if first_bracket_towards_right {
            Self::brackets_for_value_direction_fixed(slice, value, true, less_than)
        } else {
            Self::brackets_for_value_direction_fixed(slice, value, false, |a, b| less_than(b, a))
        }
    }

    /// Implements the part of 'brackets_for_value' when the direction has been decided on. 
    fn brackets_for_value_direction_fixed<T>(
        slice: &[T],
        value: &T,
        first_bracket_towards_right: bool,
        less_than: impl Fn(&T, &T) -> bool
    ) -> Self {
        std::assert!(slice.len() >= 2);
        std::assert!(!less_than(value, &slice[0]));

        let mut range_begin = 0;
        let mut range_end = slice.len();
        let mut range_begin_descending = less_than(&slice[range_begin], &slice[range_end - 1]);
        
        while range_begin + 1 < range_end {
            let range_mid = (range_begin + range_end) / 2;

            if less_than(value, &slice[range_mid]) {
                return Self::Brackets {
                    first_bracket_towards_right: first_bracket_towards_right,
                    begin_index: range_begin,
                    mid_index: range_mid,
                    end_index: range_end,
                };
            }

            if less_than(&slice[range_mid], &slice[range_begin]) {
                if range_begin_descending {
                    range_begin = range_mid;
                    range_begin_descending = less_than(&slice[range_mid], &slice[range_mid - 1]);
                } else {
                    range_end = range_mid;
                }
            } else {
                if less_than(&slice[range_mid], &slice[range_mid - 1]) {
                    range_end = range_mid;
                } else {
                    range_begin = range_mid;
                    range_begin_descending = false;
                    
                }
            }
        }

        if less_than(&slice[range_begin], value) {
            Self::ValueOutsideRange
        } else {
            Self::ValueIsExtremalElement {
                towards_right: first_bracket_towards_right,
                index: range_begin,
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_pred_modulo() {
        let mut a = 4;
        for _ in 0..2 {
            for i in (0..5).rev() {
                std::assert_eq!(a, i);
                a = pred_modulo(a, 5);
            }
        }

    }

    #[test]
    fn test_succ_modulo() {
        let mut a = 0;
        for _ in 0..2 {
            for i in 0..5 {
                std::assert_eq!(a, i);
                a = succ_modulo(a, 5);
            }
        }
    }

    #[test]
    fn test_add_modulo() {
        let m = 7;
        for i in 0..m {
            for j in 0..=m {
                std::assert_eq!(add_modulo(i, j, m), (i + j) % m);
            }
        }
    }

    #[test]
    fn test_sub_modulo() {
        let m = 7;
        for i in 0..m {
            for j in 0..=m {
                std::assert_eq!(sub_modulo(i, j, m), (m + i - j) % m);
            }
        }
    }

    #[test]
    fn test_unimodal_cyclic_sequence_maximum() {
        fn test(slice: &mut [i32], mut expected_maximum_index: usize) {
            for _ in 0..slice.len() {
                std::assert_eq!(unimodal_cyclic_sequence_maximum(slice, |a, b| *a < *b), expected_maximum_index);
                slice.rotate_right(1);
                expected_maximum_index = succ_modulo(expected_maximum_index, slice.len());
            }
        }

        // General case
        test(&mut [0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1], 6);

        // First iteration takes branch 1.
        test(&mut [10, 9, 8, 7, 1, 0, 1, 12, 11], 7);

        // First iteration takes branch 2.
        test(&mut [10, 11, 7, 2, 1, 0, 3, 6, 9], 1);

        // First iteration takes branch 3.
        test(&mut [0, 7, 12, 10, 9, 7, 5, 3, 1], 2);

        // First iteration takes branch 4.
        test(&mut [0, 3, 6, 8, 9, 10, 11, 3, 1], 6);

        // Duplicated min and max
        test(&mut [10, 7, 5, 3, 0, 0, 0, 3, 5, 7, 10, 10], 0);

        // 2 elems
        test(&mut [0, 10], 1);
    }

    fn validate_unimodal_cyclic_sequence_brackets(
        slice: &[i32],
        value: i32,
        result: UnimodalCyclicSequenceBrackets
    ) {
        match result {
            UnimodalCyclicSequenceBrackets::Brackets {
                first_bracket_towards_right, 
                begin_index,
                mid_index,
                end_index
            } => {
                let mut first_interval_index = usize::MAX; 
                let mut second_interval_index = usize::MAX;
                

                if first_bracket_towards_right {
                    for i in 0..slice.len() {
                    if slice[i] <= value && value < slice[succ_modulo(i, slice.len())] {
                        first_interval_index = i;
                    }

                    if slice[succ_modulo(i, slice.len())] <= value && value < slice[i] {
                        second_interval_index = i;
                    } 
                }
                } else {
                    for i in 0..slice.len() {
                    if slice[i] >= value && value > slice[succ_modulo(i, slice.len())] {
                        first_interval_index = i;
                    }

                    if slice[succ_modulo(i, slice.len())] >= value && value > slice[i] {
                        second_interval_index = i;
                    } 
                }
                }

                std::assert!(first_interval_index != usize::MAX);
                std::assert!(second_interval_index != usize::MAX);                

                std::assert!(begin_index <= first_interval_index);
                std::assert!(first_interval_index < mid_index);
                std::assert!(mid_index <= second_interval_index);
                std::assert!(second_interval_index < end_index);
                std::assert!(end_index <= slice.len());
            },
            UnimodalCyclicSequenceBrackets::ValueIsExtremalElement { towards_right, index }=> {
                if towards_right {
                    std::assert!(slice[0] < value);
                    std::assert!(slice[index] == value);
                    let mut max_index = 1;
                    for i in 1..slice.len() {
                        if slice[i] >= slice[max_index] {
                            max_index = i;
                        }
                    }

                    std::assert!(max_index == index);
                } else {
                    std::assert!(slice[0] > value);
                    std::assert!(slice[index] == value);
                    let mut min_index = 1;
                    for i in 1..slice.len() {
                        if slice[i] <= slice[min_index] {
                            min_index = i;
                        }
                    }

                    std::assert!(min_index == index);
                }
            },
            UnimodalCyclicSequenceBrackets::ValueOutsideRange => {
                let min_value = *slice.iter().min().unwrap();
                let max_value = *slice.iter().max().unwrap();
                std::assert!(value < min_value || value > max_value)
            },
        }
    }

    #[test]
    fn test_brackets_for_value_direction_fixed() {
        fn test(slice: &[i32], value: i32) {
            let result = UnimodalCyclicSequenceBrackets::brackets_for_value_direction_fixed(
                slice,
                &value,
                true,
                |a, b| *a < *b
            );
            
            validate_unimodal_cyclic_sequence_brackets(slice, value, result);
        }

        // First iteration mid already to right.
        test(&[0, 10, 20, 30, 40, 50, 40, 30, 20, 10], 45);

        // slice[0] in ascending part, first slice[mid] in ascending part between slice[0] and value.
        test(&[3, 4, 5, 6, 7, 8, 10, 5], 9);

        // slice[0] in ascending part, first slice[mid] in descending part between slice[0] and value.
        test(&[-3, 15, 4, 2, 0, -2, -3, -5, -4], 5);

        // slice[0] in ascending part, first slice[mid] to left of slice[0].
        test(&[-3, 2, 8, -4, -7, -9, -7, -6], -1);
        
        // slice[0] in descending part, first slice[mid] to left of slice[0].
        test(&[-4, -6, -7, -6, -5, 4, 8, 1], 2);

        // slice[0] in descending part, first slice[mid] in descending part, between slice[0] and value.
        test(&[-2, -6, 12, 9, 6, 4, 3, 1], 10);

        // slice[0] in descending part, first slice[mid] in ascending part between slice[0] and value.
        test(&[-2, -6, 2, 4, 6, 10, 1], 8);

        // Equal to max elem
        test(&[-8, -6, -3, 1, 2, 5, 8, 9, 7, 4, 2, -4, -7], 9);

        // To right of max elem
        test(&[1, 5, 8, 9, 4], 11);
    }

    #[test]
    fn test_brackets_for_value() {
        let mut elems = [6, 21, 28, 34, 38, 42, 46, 48, 53, 62, 65, 67, 72, 78, 85, 89, 86, 74, 58, 40, 24, 12, 10];
        for _ in 0..elems.len() {
            for i in 0..elems.len() {
                validate_unimodal_cyclic_sequence_brackets(
                    &elems,
                    elems[i],
                    UnimodalCyclicSequenceBrackets::brackets_for_value(&elems, &elems[i], |a, b| *a < *b) 
                );

                let mid_value = (elems[i] + elems[succ_modulo(i, elems.len())]) / 2;
                validate_unimodal_cyclic_sequence_brackets(
                    &elems,
                    mid_value,
                    UnimodalCyclicSequenceBrackets::brackets_for_value(&elems, &mid_value, |a, b| *a < *b) 
                );
            }

            validate_unimodal_cyclic_sequence_brackets(
                &elems,
                -11,
                UnimodalCyclicSequenceBrackets::brackets_for_value(&elems, &-11, |a, b| *a < *b)
            );

            validate_unimodal_cyclic_sequence_brackets(
                &elems,
                100,
                UnimodalCyclicSequenceBrackets::brackets_for_value(&elems, &100, |a, b| *a < *b)
            );

            elems.rotate_right(1);
        }
    }
}
