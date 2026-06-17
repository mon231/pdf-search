# PDF Search Project

This project is a Rust-based tool for searching PDFs to find specific images. It scans through PDF files in a specified directory and compares them to a target image to identify which PDF contains the image.

## Requirements

- Rust and Cargo installed

## Setup

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```

2. Navigate to the project directory:
   ```bash
   cd pdf_search
   ```

3. Build the project:
   ```bash
   cargo build --release
   ```

## Usage

To use the tool, run the following command:

```bash
cargo run --release -- <path-to-pdf-directory> <path-to-target-image>
```

- `<path-to-pdf-directory>`: Directory containing the PDFs to search.
- `<path-to-target-image>`: Path to the image to search for in the PDFs.

## Example

```bash
cargo run --release -- C:\Users\avart\Desktop\ComputerSciense\compilation\Exams C:\Users\avart\Desktop\ComputerSciense\compilation\qu_test.png
```

This will search through all PDFs in the `Exams` directory to find the one containing the `qu_test.png` image.

## License

This project is licensed under the MIT License.
