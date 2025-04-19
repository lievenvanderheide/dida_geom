use std::mem::MaybeUninit;

/// A builder type which facilitates the construction of (fixed size) arrays using a 'Vec::push' like interface.
pub struct ArrayBuilder<T, const N: usize> {
    elems: [MaybeUninit<T>; N],
    len: usize,
}

impl<T, const N: usize> ArrayBuilder<T, N> {
    /// Constructs a new ArrayBuilder.
    pub fn new() -> Self {
        Self {
            elems: [const { MaybeUninit::<T>::uninit() }; N],
            len: 0,
        }
    }

    /// Appends anw element to the part of the array already built. 
    ///
    /// This function should not be called at most 'N' times.
    pub fn push(&mut self, elem: T) {
        std::assert!(self.len < N);
        self.elems[self.len].write(elem);
        self.len += 1;
    }

    /// Builds the array.
    /// This function should be called after 'push' has been called exactly 'N' times.
    pub fn build(self) -> [T; N] {
        std::assert!(self.len == N);

        // replace with array_assume_init when it becomes stable.
        let result = unsafe {
            std::ptr::read(self.elems.as_ptr() as *const [T; N])
        };

        std::mem::forget(self);

        result
    }
}

impl<T, const N: usize> Drop for ArrayBuilder<T, N> {
    fn drop(&mut self) {
        unsafe {
            for i in 0..self.len {
                self.elems[i].assume_init_drop();
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::cell::RefCell;

    struct DropTracker<'a> {
        pub c: char,
        pub num_instances: &'a RefCell<i32>, 
    }

    impl<'a> DropTracker<'a> {
        pub fn new(c: char, num_instances: &'a RefCell<i32>) -> DropTracker<'a> {
            *num_instances.borrow_mut() += 1;
            DropTracker { c, num_instances }
        }
    }

    impl<'a> Drop for DropTracker<'a> {
        fn drop(&mut self) {
            *self.num_instances.borrow_mut() -= 1;
        }
    }

    #[test]
    fn test_build() {
        let num_instances = RefCell::<i32>::new(0);

        {
            let mut builder = ArrayBuilder::<DropTracker, 4>::new();
            builder.push(DropTracker::new('D', &num_instances));
            builder.push(DropTracker::new('i', &num_instances));
            builder.push(DropTracker::new('d', &num_instances));
            builder.push(DropTracker::new('a', &num_instances));

            let result = builder.build();
            std::assert_eq!(*num_instances.borrow(), 4);

            std::assert_eq!(result[0].c, 'D');
            std::assert_eq!(result[1].c, 'i');
            std::assert_eq!(result[2].c, 'd');
            std::assert_eq!(result[3].c, 'a');
        }

        std::assert_eq!(*num_instances.borrow(), 0);
    }

    #[test]
    fn test_drop() {
        let num_instances = RefCell::<i32>::new(0);

        {
            let mut builder = ArrayBuilder::<DropTracker, 4>::new();
            builder.push(DropTracker::new('D', &num_instances));
            builder.push(DropTracker::new('i', &num_instances));
            builder.push(DropTracker::new('d', &num_instances));

            std::assert_eq!(*num_instances.borrow(), 3);
        }

        std::assert_eq!(*num_instances.borrow(), 0);
    }
}
