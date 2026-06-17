/// Populates the `samples/` directory with ready-to-use PDF and PNG files.
/// Run with: cargo run --bin generate_samples
use image::RgbImage;
use lopdf::{Dictionary, Document, Object, Stream};
use std::path::Path;

fn gradient_rgb(width: u32, height: u32, b: u8) -> Vec<u8> {
    let mut data = Vec::with_capacity((width * height * 3) as usize);
    for y in 0..height {
        for x in 0..width {
            data.push((x * 220 / width) as u8); // R
            data.push((y * 220 / height) as u8); // G
            data.push(b); // B
        }
    }
    data
}

fn save_rgb_as_png(rgb: &[u8], width: u32, height: u32, path: &Path) {
    RgbImage::from_raw(width, height, rgb.to_vec())
        .expect("RgbImage::from_raw")
        .save(path)
        .expect("save PNG");
    println!("  wrote {:?}", path);
}

fn create_pdf_with_image(rgb: &[u8], width: u32, height: u32, path: &Path) {
    let mut doc = Document::with_version("1.5");

    let mut img_dict = Dictionary::new();
    img_dict.set("Type", Object::Name(b"XObject".to_vec()));
    img_dict.set("Subtype", Object::Name(b"Image".to_vec()));
    img_dict.set("Width", Object::Integer(width as i64));
    img_dict.set("Height", Object::Integer(height as i64));
    img_dict.set("ColorSpace", Object::Name(b"DeviceRGB".to_vec()));
    img_dict.set("BitsPerComponent", Object::Integer(8));
    let image_id = doc.add_object(Object::Stream(Stream::new(img_dict, rgb.to_vec())));

    let content = format!("q {} 0 0 {} 0 0 cm /Img Do Q", width, height);
    let mut content_dict = Dictionary::new();
    content_dict.set("Length", Object::Integer(content.len() as i64));
    let content_id =
        doc.add_object(Object::Stream(Stream::new(content_dict, content.into_bytes())));

    let pages_id = doc.new_object_id();

    let mut xobjects = Dictionary::new();
    xobjects.set("Img", Object::Reference(image_id));
    let mut resources = Dictionary::new();
    resources.set("XObject", Object::Dictionary(xobjects));

    let mut page_dict = Dictionary::new();
    page_dict.set("Type", Object::Name(b"Page".to_vec()));
    page_dict.set("Parent", Object::Reference(pages_id));
    page_dict.set(
        "MediaBox",
        Object::Array(vec![
            Object::Integer(0),
            Object::Integer(0),
            Object::Integer(width as i64),
            Object::Integer(height as i64),
        ]),
    );
    page_dict.set("Contents", Object::Reference(content_id));
    page_dict.set("Resources", Object::Dictionary(resources));
    let page_id = doc.add_object(Object::Dictionary(page_dict));

    let mut pages_dict = Dictionary::new();
    pages_dict.set("Type", Object::Name(b"Pages".to_vec()));
    pages_dict.set("Kids", Object::Array(vec![Object::Reference(page_id)]));
    pages_dict.set("Count", Object::Integer(1));
    doc.objects
        .insert(pages_id, Object::Dictionary(pages_dict));

    let mut catalog_dict = Dictionary::new();
    catalog_dict.set("Type", Object::Name(b"Catalog".to_vec()));
    catalog_dict.set("Pages", Object::Reference(pages_id));
    let catalog_id = doc.add_object(Object::Dictionary(catalog_dict));
    doc.trailer.set("Root", Object::Reference(catalog_id));

    doc.save(path).expect("save PDF");
    println!("  wrote {:?}", path);
}

fn main() {
    let out = Path::new("samples");
    std::fs::create_dir_all(out).expect("create samples/");

    const W: u32 = 64;
    const H: u32 = 64;

    println!("image_a.png  →  contains_image_a.pdf");
    let img_a = gradient_rgb(W, H, 40);
    save_rgb_as_png(&img_a, W, H, &out.join("image_a.png"));
    create_pdf_with_image(&img_a, W, H, &out.join("contains_image_a.pdf"));

    println!("image_b.png  →  contains_image_b.pdf");
    let img_b = gradient_rgb(W, H, 210);
    save_rgb_as_png(&img_b, W, H, &out.join("image_b.png"));
    create_pdf_with_image(&img_b, W, H, &out.join("contains_image_b.pdf"));

    println!("(no matching PNG)  →  no_match.pdf");
    let img_c = gradient_rgb(W, H, 128);
    create_pdf_with_image(&img_c, W, H, &out.join("no_match.pdf"));

    println!();
    println!("Done. Example usage:");
    println!("  cargo run -- samples samples/image_a.png");
}
