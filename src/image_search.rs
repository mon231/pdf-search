use crate::pdf_processing::{extract_images_from_page, load_pdf};
use glob::glob;
use image::{DynamicImage, ImageReader, RgbImage};
use rayon::prelude::*;
use std::path::{Path, PathBuf};

/// Search every PDF in `pdf_dir` for the image at `image_path`.
/// PDFs are scanned in parallel; within each PDF pages are scanned in order
/// with early exit on the first match.  Results are sorted by path.
pub fn search_image_in_pdfs(image_path: &Path, pdf_dir: &Path) -> Result<Vec<PathBuf>, String> {
    let target = load_target(image_path)?;
    // Pre-convert once so each parallel worker reuses the same buffer.
    let target_rgb = target.to_rgb8();

    let pattern = format!("{}/*.pdf", pdf_dir.display());
    let pdf_paths: Vec<PathBuf> = glob(&pattern)
        .map_err(|e| e.to_string())?
        .filter_map(|e| e.ok())
        .collect();

    let mut matches: Vec<PathBuf> = pdf_paths
        .into_par_iter()
        .filter(|pdf_path| {
            load_pdf(pdf_path)
                .map(|doc| pdf_contains_image(&doc, &target_rgb))
                .unwrap_or(false)
        })
        .collect();

    matches.sort(); // deterministic order regardless of scheduling
    Ok(matches)
}

fn load_target(path: &Path) -> Result<DynamicImage, String> {
    ImageReader::open(path)
        .map_err(|e| format!("Cannot open {:?}: {}", path, e))?
        .decode()
        .map_err(|e| format!("Cannot decode {:?}: {}", path, e))
}

fn pdf_contains_image(doc: &lopdf::Document, target: &RgbImage) -> bool {
    for (_page_num, page_id) in doc.get_pages() {
        for extracted in extract_images_from_page(doc, page_id) {
            if image_matches(&extracted, target) {
                return true; // early exit: no need to check remaining pages
            }
        }
    }
    false
}

/// Mean-squared RGB error < 25.0 (RMSE < 5 per channel) counts as a match.
/// This tolerates lossless round-trips and minor compression artefacts while
/// cleanly rejecting genuinely different images.
fn image_matches(source: &DynamicImage, target: &RgbImage) -> bool {
    if source.width() != target.width() || source.height() != target.height() {
        return false;
    }

    let src = source.to_rgb8();
    let sum_sq: u64 = src
        .pixels()
        .zip(target.pixels())
        .map(|(a, b)| {
            a.0.iter()
                .zip(b.0.iter())
                .map(|(&x, &y)| {
                    let d = x as i64 - y as i64;
                    (d * d) as u64
                })
                .sum::<u64>()
        })
        .sum();

    let n = (src.width() * src.height() * 3) as f64;
    (sum_sq as f64 / n) < 25.0
}
