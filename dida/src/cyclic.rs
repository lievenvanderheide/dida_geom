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
pub fn unimodal_cyclic_sequence_maximum<T>(slice: &[T], greater_than: impl Fn(&T, &T) -> bool) -> usize {
    std::assert!(slice.len() >= 2);

    let mut range_begin = 0;
    let mut range_end = slice.len();
    let mut range_begin_descending = greater_than(&slice[range_end - 1], &slice[range_begin]);
    
    while range_begin + 1 < range_end {
        let range_mid = (range_begin + range_end) / 2;

        if greater_than(&slice[range_begin], &slice[range_mid]) {
            if range_begin_descending {
                range_begin = range_mid;
                range_begin_descending = greater_than(&slice[range_mid - 1], &slice[range_mid]);
            } else {
                range_end = range_mid;
            }
        } else {
            if greater_than(&slice[range_mid - 1], &slice[range_mid]) {
                range_end = range_mid;
            } else {
                range_begin = range_mid;
                range_begin_descending = false;
                
            }
        }
    }

    range_begin
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
                std::assert_eq!(unimodal_cyclic_sequence_maximum(slice, |a, b| *a > *b), expected_maximum_index);
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
}
