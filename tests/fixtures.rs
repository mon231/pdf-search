use image::RgbImage;
use lopdf::{Dictionary, Document, Object, ObjectId, Stream};
use std::path::Path;

/// RGB gradient image: x→red, y→green, `b` is fixed blue.
/// Different `b` values make images statistically distinct for MSE comparison.
pub fn gradient_rgb(width: u32, height: u32, b: u8) -> Vec<u8> {
    let mut data = Vec::with_capacity((width * height * 3) as usize);
    for y in 0..height {
        for x in 0..width {
            data.push((x * 220 / width) as u8); // R
            data.push((y * 220 / height) as u8); // G
            data.push(b); // B — unique per image
        }
    }
    data
}

/// Save a raw RGB byte buffer as a PNG file.
pub fn save_rgb_as_png(rgb: &[u8], width: u32, height: u32, path: &Path) {
    RgbImage::from_raw(width, height, rgb.to_vec())
        .expect("RgbImage::from_raw")
        .save(path)
        .expect("save PNG");
}

// ── single-page PDF ──────────────────────────────────────────────────────────

/// Create a one-page PDF with a single uncompressed RGB image XObject.
pub fn create_pdf_with_image(rgb: &[u8], width: u32, height: u32, path: &Path) {
    let mut doc = Document::with_version("1.5");
    let image_id = add_image_xobject(&mut doc, rgb, width, height);
    let pages_id = doc.new_object_id();
    let page_id = add_image_page(&mut doc, pages_id, image_id, width, height);
    finish_doc(&mut doc, pages_id, vec![page_id], path);
}

// ── multi-page PDF ───────────────────────────────────────────────────────────

/// Create a PDF with `total_pages` pages where the target image appears only
/// on page `image_page` (0-indexed).  All other pages are blank.
pub fn create_multipage_pdf_with_image_on_page(
    image_page: usize,
    total_pages: usize,
    rgb: &[u8],
    width: u32,
    height: u32,
    path: &Path,
) {
    assert!(image_page < total_pages, "image_page must be < total_pages");

    let mut doc = Document::with_version("1.5");
    let image_id = add_image_xobject(&mut doc, rgb, width, height);
    let pages_id = doc.new_object_id();

    let mut page_ids = Vec::with_capacity(total_pages);
    for i in 0..total_pages {
        let page_id = if i == image_page {
            add_image_page(&mut doc, pages_id, image_id, width, height)
        } else {
            add_blank_page(&mut doc, pages_id)
        };
        page_ids.push(page_id);
    }

    finish_doc(&mut doc, pages_id, page_ids, path);
}

// ── internal helpers ─────────────────────────────────────────────────────────

fn add_image_xobject(doc: &mut Document, rgb: &[u8], width: u32, height: u32) -> ObjectId {
    let mut d = Dictionary::new();
    d.set("Type", Object::Name(b"XObject".to_vec()));
    d.set("Subtype", Object::Name(b"Image".to_vec()));
    d.set("Width", Object::Integer(width as i64));
    d.set("Height", Object::Integer(height as i64));
    d.set("ColorSpace", Object::Name(b"DeviceRGB".to_vec()));
    d.set("BitsPerComponent", Object::Integer(8));
    doc.add_object(Object::Stream(Stream::new(d, rgb.to_vec())))
}

fn add_image_page(
    doc: &mut Document,
    pages_id: ObjectId,
    image_id: ObjectId,
    width: u32,
    height: u32,
) -> ObjectId {
    let content = format!("q {} 0 0 {} 0 0 cm /Img Do Q", width, height);
    let mut cd = Dictionary::new();
    cd.set("Length", Object::Integer(content.len() as i64));
    let content_id = doc.add_object(Object::Stream(Stream::new(cd, content.into_bytes())));

    let mut xobjects = Dictionary::new();
    xobjects.set("Img", Object::Reference(image_id));
    let mut resources = Dictionary::new();
    resources.set("XObject", Object::Dictionary(xobjects));

    let mut p = Dictionary::new();
    p.set("Type", Object::Name(b"Page".to_vec()));
    p.set("Parent", Object::Reference(pages_id));
    p.set(
        "MediaBox",
        Object::Array(vec![
            Object::Integer(0),
            Object::Integer(0),
            Object::Integer(width as i64),
            Object::Integer(height as i64),
        ]),
    );
    p.set("Contents", Object::Reference(content_id));
    p.set("Resources", Object::Dictionary(resources));
    doc.add_object(Object::Dictionary(p))
}

fn add_blank_page(doc: &mut Document, pages_id: ObjectId) -> ObjectId {
    let mut cd = Dictionary::new();
    cd.set("Length", Object::Integer(0));
    let content_id = doc.add_object(Object::Stream(Stream::new(cd, vec![])));

    let mut p = Dictionary::new();
    p.set("Type", Object::Name(b"Page".to_vec()));
    p.set("Parent", Object::Reference(pages_id));
    p.set(
        "MediaBox",
        Object::Array(vec![
            Object::Integer(0),
            Object::Integer(0),
            Object::Integer(595), // A4 points
            Object::Integer(842),
        ]),
    );
    p.set("Contents", Object::Reference(content_id));
    p.set("Resources", Object::Dictionary(Dictionary::new()));
    doc.add_object(Object::Dictionary(p))
}

fn finish_doc(doc: &mut Document, pages_id: ObjectId, page_ids: Vec<ObjectId>, path: &Path) {
    let count = page_ids.len() as i64;
    let kids: Vec<Object> = page_ids.into_iter().map(Object::Reference).collect();

    let mut pages = Dictionary::new();
    pages.set("Type", Object::Name(b"Pages".to_vec()));
    pages.set("Kids", Object::Array(kids));
    pages.set("Count", Object::Integer(count));
    doc.objects.insert(pages_id, Object::Dictionary(pages));

    let mut catalog = Dictionary::new();
    catalog.set("Type", Object::Name(b"Catalog".to_vec()));
    catalog.set("Pages", Object::Reference(pages_id));
    let catalog_id = doc.add_object(Object::Dictionary(catalog));
    doc.trailer.set("Root", Object::Reference(catalog_id));

    doc.save(path).expect("save PDF");
}
