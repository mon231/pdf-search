use image::{DynamicImage, GrayImage, RgbImage};
use lopdf::{Document, Object, ObjectId};
use std::path::Path;

pub fn load_pdf(path: &Path) -> Result<Document, String> {
    Document::load(path).map_err(|e| format!("Failed to load PDF: {}", e))
}

pub fn extract_images_from_page(doc: &Document, page_id: ObjectId) -> Vec<DynamicImage> {
    let mut images = Vec::new();

    let page_obj = match doc.get_object(page_id) {
        Ok(o) => o,
        Err(_) => return images,
    };
    let page_dict = match page_obj.as_dict() {
        Ok(d) => d,
        Err(_) => return images,
    };

    let resources_obj = match page_dict.get(b"Resources") {
        Ok(r) => r,
        Err(_) => return images,
    };
    let resources_dict = match resolve_dict(doc, resources_obj) {
        Some(d) => d,
        None => return images,
    };

    let xobjects_obj = match resources_dict.get(b"XObject") {
        Ok(x) => x,
        Err(_) => return images,
    };
    let xobjects_dict = match resolve_dict(doc, xobjects_obj) {
        Some(d) => d,
        None => return images,
    };

    for (_name, xobj_ref) in xobjects_dict.iter() {
        let xobj = match xobj_ref {
            Object::Reference(id) => match doc.get_object(*id) {
                Ok(o) => o,
                Err(_) => continue,
            },
            other => other,
        };
        if let Some(img) = extract_image(xobj) {
            images.push(img);
        }
    }

    images
}

fn resolve_dict<'a>(doc: &'a Document, obj: &'a Object) -> Option<&'a lopdf::Dictionary> {
    match obj {
        Object::Dictionary(d) => Some(d),
        Object::Reference(id) => doc.get_object(*id).ok()?.as_dict().ok(),
        _ => None,
    }
}

fn extract_image(obj: &Object) -> Option<DynamicImage> {
    let stream = match obj {
        Object::Stream(s) => s,
        _ => return None,
    };

    let subtype = stream.dict.get(b"Subtype").ok()?.as_name_str().ok()?;
    if subtype != "Image" {
        return None;
    }

    let width = stream.dict.get(b"Width").ok()?.as_i64().ok()? as u32;
    let height = stream.dict.get(b"Height").ok()?.as_i64().ok()? as u32;

    let color_space = stream
        .dict
        .get(b"ColorSpace")
        .ok()
        .and_then(|cs| cs.as_name_str().ok())
        .unwrap_or("DeviceRGB");

    let data = &stream.content;

    match color_space {
        "DeviceGray" => {
            let expected = (width * height) as usize;
            if data.len() < expected {
                return None;
            }
            let gray = GrayImage::from_raw(width, height, data[..expected].to_vec())?;
            Some(DynamicImage::ImageLuma8(gray))
        }
        _ => {
            let expected = (width * height * 3) as usize;
            if data.len() < expected {
                return None;
            }
            let rgb = RgbImage::from_raw(width, height, data[..expected].to_vec())?;
            Some(DynamicImage::ImageRgb8(rgb))
        }
    }
}
