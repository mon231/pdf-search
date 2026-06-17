mod fixtures;
use fixtures::{
    create_multipage_pdf_with_image_on_page, create_pdf_with_image, gradient_rgb, save_rgb_as_png,
};

use pdf_image_search::image_search::search_image_in_pdfs;
use std::time::Instant;
use tempfile::TempDir;

const W: u32 = 24;
const H: u32 = 24;

// ── single-page correctness ───────────────────────────────────────────────────

/// Only the PDF embedding image_a is returned when searching for image_a.
#[test]
fn test_finds_correct_pdf() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let img_a = gradient_rgb(W, H, 40);
    let img_b = gradient_rgb(W, H, 210);

    let png_a = dir.join("image_a.png");
    save_rgb_as_png(&img_a, W, H, &png_a);

    create_pdf_with_image(&img_a, W, H, &dir.join("contains_a.pdf"));
    create_pdf_with_image(&img_b, W, H, &dir.join("contains_b.pdf"));

    let results = search_image_in_pdfs(&png_a, dir).unwrap();

    assert_eq!(results.len(), 1, "expected exactly one matching PDF");
    assert_eq!(results[0].file_name().unwrap(), "contains_a.pdf");
}

/// A PNG not embedded in any PDF returns an empty list.
#[test]
fn test_no_match_returns_empty() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let img_a = gradient_rgb(W, H, 40);
    let img_b = gradient_rgb(W, H, 210);
    let img_c = gradient_rgb(W, H, 128); // not embedded anywhere

    save_rgb_as_png(&img_c, W, H, &dir.join("image_c.png"));
    create_pdf_with_image(&img_a, W, H, &dir.join("has_a.pdf"));
    create_pdf_with_image(&img_b, W, H, &dir.join("has_b.pdf"));

    let results = search_image_in_pdfs(&dir.join("image_c.png"), dir).unwrap();
    assert!(results.is_empty(), "expected no matching PDFs");
}

/// The same image embedded in three PDFs — all three are returned, sorted.
#[test]
fn test_finds_all_pdfs_with_shared_image() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let img = gradient_rgb(W, H, 128);
    save_rgb_as_png(&img, W, H, &dir.join("shared.png"));

    create_pdf_with_image(&img, W, H, &dir.join("pdf_1.pdf"));
    create_pdf_with_image(&img, W, H, &dir.join("pdf_2.pdf"));
    create_pdf_with_image(&img, W, H, &dir.join("pdf_3.pdf"));

    let results = search_image_in_pdfs(&dir.join("shared.png"), dir).unwrap();
    assert_eq!(results.len(), 3);
    assert_eq!(results[0].file_name().unwrap(), "pdf_1.pdf");
    assert_eq!(results[1].file_name().unwrap(), "pdf_2.pdf");
    assert_eq!(results[2].file_name().unwrap(), "pdf_3.pdf");
}

/// Each PNG matches exactly its own PDF — no cross-matches.
#[test]
fn test_each_image_matches_its_own_pdf() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let cases: &[(u8, &str, &str)] = &[
        (30, "img_x.png", "pdf_x.pdf"),
        (130, "img_y.png", "pdf_y.pdf"),
        (220, "img_z.png", "pdf_z.pdf"),
    ];

    for (b, png, pdf) in cases {
        let data = gradient_rgb(W, H, *b);
        save_rgb_as_png(&data, W, H, &dir.join(png));
        create_pdf_with_image(&data, W, H, &dir.join(pdf));
    }

    for (_, png, pdf) in cases {
        let results = search_image_in_pdfs(&dir.join(png), dir).unwrap();
        assert_eq!(results.len(), 1, "PNG {png} should match exactly one PDF");
        assert_eq!(results[0].file_name().unwrap(), *pdf);
    }
}

// ── multi-page PDF correctness ────────────────────────────────────────────────

/// Image on the very first page of a 5-page PDF is found.
#[test]
fn test_multipage_image_on_first_page() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let img = gradient_rgb(W, H, 77);
    save_rgb_as_png(&img, W, H, &dir.join("target.png"));

    create_multipage_pdf_with_image_on_page(0, 5, &img, W, H, &dir.join("multipage.pdf"));

    let results = search_image_in_pdfs(&dir.join("target.png"), dir).unwrap();
    assert_eq!(results.len(), 1);
    assert_eq!(results[0].file_name().unwrap(), "multipage.pdf");
}

/// Image buried on the last page of a 5-page PDF is still found.
#[test]
fn test_multipage_image_on_last_page() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let img = gradient_rgb(W, H, 77);
    save_rgb_as_png(&img, W, H, &dir.join("target.png"));

    create_multipage_pdf_with_image_on_page(4, 5, &img, W, H, &dir.join("multipage.pdf"));

    let results = search_image_in_pdfs(&dir.join("target.png"), dir).unwrap();
    assert_eq!(results.len(), 1);
    assert_eq!(results[0].file_name().unwrap(), "multipage.pdf");
}

/// Target not present on any of the 5 pages — no PDF returned.
#[test]
fn test_multipage_no_image_on_any_page() {
    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let target = gradient_rgb(W, H, 77);
    let decoy = gradient_rgb(W, H, 200);

    save_rgb_as_png(&target, W, H, &dir.join("target.png"));
    create_multipage_pdf_with_image_on_page(2, 5, &decoy, W, H, &dir.join("multipage.pdf"));

    let results = search_image_in_pdfs(&dir.join("target.png"), dir).unwrap();
    assert!(results.is_empty());
}

// ── scale / performance ───────────────────────────────────────────────────────

/// 40 PDFs × 8 pages = 320 pages searched in parallel.
/// Only the one PDF that embeds the target on page 5 is returned.
/// Prints elapsed time so regression is visible in CI output.
#[test]
fn test_many_pdfs_many_pages_correctness() {
    const N_PDFS: usize = 40;
    const PAGES: usize = 8;
    const TARGET_IDX: usize = 19; // 0-indexed
    const TARGET_PAGE: usize = 5;

    let tmp = TempDir::new().unwrap();
    let dir = tmp.path();

    let target_img = gradient_rgb(W, H, 90);
    let decoy_img = gradient_rgb(W, H, 180);

    save_rgb_as_png(&target_img, W, H, &dir.join("target.png"));

    for i in 0..N_PDFS {
        let pdf_name = format!("doc_{:03}.pdf", i);
        if i == TARGET_IDX {
            create_multipage_pdf_with_image_on_page(
                TARGET_PAGE,
                PAGES,
                &target_img,
                W,
                H,
                &dir.join(&pdf_name),
            );
        } else {
            create_multipage_pdf_with_image_on_page(
                0,
                PAGES,
                &decoy_img,
                W,
                H,
                &dir.join(&pdf_name),
            );
        }
    }

    let t0 = Instant::now();
    let results = search_image_in_pdfs(&dir.join("target.png"), dir).unwrap();
    let elapsed = t0.elapsed();

    println!(
        "Searched {} PDFs × {} pages = {} pages in {:.2?}",
        N_PDFS,
        PAGES,
        N_PDFS * PAGES,
        elapsed
    );

    assert_eq!(results.len(), 1, "expected exactly one matching PDF");
    let name = results[0].file_name().unwrap().to_str().unwrap();
    assert_eq!(
        name,
        format!("doc_{:03}.pdf", TARGET_IDX),
        "wrong PDF returned"
    );
}
