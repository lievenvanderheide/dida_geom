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
}
