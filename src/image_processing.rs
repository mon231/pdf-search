use image::{DynamicImage, ImageReader};
use std::path::Path;

pub fn load_image(path: &Path) -> Result<DynamicImage, String> {
    ImageReader::open(path)
        .map_err(|e| format!("Failed to open image: {}", e))?
        .decode()
        .map_err(|e| format!("Failed to decode image: {}", e))
}
