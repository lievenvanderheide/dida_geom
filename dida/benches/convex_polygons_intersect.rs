use dida::{ConvexPolygon, Intersects};
use std::str::FromStr;
use dida::Vec2;

fn main() {
    divan::main();
}

#[divan::bench]
fn convex_polygons_intersect(bencher: divan::Bencher) {
    let a = ConvexPolygon::from_str("{
        {-5.28, 2.64}, {-4.92, 0.60}, {-3.70, -1.46}, {-2.06, -3.04}, {-0.10, -3.42}, {2.24, -3.12}, {4.20, -2.16},
        {5.58, -0.90}, {5.12, 1.58}, {4.12, 4.08}, {2.60, 5.68}, {0.44, 7.26}, {-0.84, 7.54}, {-2.78, 6.86},
        {-3.70, 6.14}, {-4.66, 4.46}
    }").unwrap();

    let b = ConvexPolygon::from_str("{
        {0.24, -6.02}, {1.60, -8.10}, {3.82, -9.26}, {6.40, -9.38}, {8.30, -9.30}, {9.56, -8.20}, {10.32, -6.38},
        {10.58, -4.66}, {9.84, -3.04}, {9.00, -1.58}, {7.24, -1.02}, {5.94, -1.46}, {3.62, -2.62}, {2.12, -3.70},
        {1.34, -4.34}, {0.74, -4.98}
    }").unwrap();

    bencher.bench(|| {
        divan::black_box(&a).intersects(divan::black_box(&b))
    });
}

#[divan::bench]
fn cross_product(bencher: divan::Bencher) {
    let a = Vec2::new(1.23, 4.56);
    let b = Vec2::new(1.23, 4.56);
    bencher.bench(|| {
        Vec2::cross(divan::black_box(a), divan::black_box(b))
    });
}
