use pdf_image_search::image_search::search_image_in_pdfs;
use std::path::Path;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 3 {
        eprintln!("Usage: {} <pdf-directory> <target-image.png>", args[0]);
        std::process::exit(1);
    }

    let pdf_dir = Path::new(&args[1]);
    let target_image = Path::new(&args[2]);

    match search_image_in_pdfs(target_image, pdf_dir) {
        Ok(matching_pdfs) if matching_pdfs.is_empty() => {
            println!("No PDFs found containing the target image.");
        }
        Ok(matching_pdfs) => {
            println!("Found target image in {} PDF(s):", matching_pdfs.len());
            for pdf in matching_pdfs {
                println!("  {:?}", pdf);
            }
        }
        Err(e) => eprintln!("Error: {}", e),
    }
}
