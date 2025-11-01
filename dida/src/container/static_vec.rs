use std::mem::MaybeUninit;
use std::fmt::Debug;
use std::ops::{Index, IndexMut};

/// A vector with a fixed capacity, which stores its element inline in the object.
pub struct StaticVec<T, const CAPACITY: usize> {
    elems: [MaybeUninit<T>; CAPACITY],
    len: usize,
}

impl<T, const CAPACITY: usize> StaticVec<T, CAPACITY> {
    /// Constructs an empty `StaticVec`.
    pub fn new() -> StaticVec<T, CAPACITY> {
        StaticVec {
            elems: [const { MaybeUninit::<T>::uninit() }; CAPACITY],
            len: 0,
        }
    }

    /// Pushes a new element at the end of the vector.
    ///
    /// This function will panic if there's no capacity left in the vector.
    pub fn push(&mut self, elem: T) {
        std::assert!(self.len < CAPACITY);
        self.elems[self.len].write(elem);
        self.len += 1;
    }

    /// Pops the element at the end of the vector, and returns the value. Returns `None` if the vector is empty. 
    pub fn pop(&mut self) -> Option<T> {
        if self.len != 0 {
            self.len -= 1;
            unsafe {
                Some(self.elems[self.len].assume_init_read()) 
            }
        } else {
            None
        }
    }

    /// Returns the number of elements in this vector.
    pub fn len(&self) -> usize {
        self.len
    }
}

impl<T, const CAPACITY: usize> Drop for StaticVec<T, CAPACITY> {
    fn drop(&mut self) {
        unsafe {
            for i in 0..self.len {
                self.elems[i].assume_init_drop();
            }
        }
    }
}

impl<T, const CAPACITY: usize> Index<usize> for StaticVec<T, CAPACITY> {
    type Output = T;

    fn index(&self, index: usize) -> &Self::Output {
        std::assert!(index < self.len);
        unsafe {
            self.elems[index].assume_init_ref()
        }
    }
}

impl<T, const CAPACITY: usize> IndexMut<usize> for StaticVec<T, CAPACITY> {    
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        std::assert!(index < self.len);
        unsafe {
            self.elems[index].assume_init_mut()
        }
    }
}

impl<T: Debug, const CAPACITY: usize> Debug for StaticVec<T, CAPACITY> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let as_slice = unsafe {
            std::slice::from_raw_parts(self.elems.as_ptr() as *const T, self.len)
        };

        Debug::fmt(&as_slice, f)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_usage() {
        let mut vec = StaticVec::<i32, 5>::new();
        std::assert_eq!(vec.len(), 0);

        vec.push(51);
        std::assert_eq!(vec.len(), 1);
        std::assert_eq!(vec[0], 51);

        vec.push(64);
        std::assert_eq!(vec.len(), 2);
        std::assert_eq!(vec[1], 64);

        vec[0] = 45;
        std::assert_eq!(vec.len(), 2);
        std::assert_eq!(vec[0], 45);
        std::assert_eq!(vec[1], 64);

        std::assert_eq!(vec.pop(), Some(64));
        std::assert_eq!(vec.len(), 1);
        std::assert_eq!(vec[0], 45);

        vec.push(32);
        std::assert_eq!(vec.len(), 2);
        std::assert_eq!(vec[1], 32);

        std::assert_eq!(vec.pop(), Some(32));
        std::assert_eq!(vec.len(), 1);

        std::assert_eq!(vec.pop(),Some(45));
        std::assert_eq!(vec.len(), 0);

        std::assert_eq!(vec.pop(), None);
    }

    #[test]
    fn test_debug_fmt() {
        let mut vec = StaticVec::<i32, 8>::new();
        vec.push(73);
        vec.push(64);
        vec.push(82);
        
        std::assert_eq!(format!("{:?}", vec), format!("{:?}", [73, 64, 82]));
    }
}
