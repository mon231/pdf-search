import cv2
from pathlib import Path
from pdf2image import pdf2image
from argparse import ArgumentParser
from tempfile import TemporaryDirectory


def generate_splitted_pdf(root_dir: Path):
    for pdf_path in root_dir.rglob('*.pdf'):
        with TemporaryDirectory() as extracted_pages_root:
            pdf2image.convert_from_path(pdf_path, output_folder=extracted_pages_root, dpi=200, fmt="png")
            yield pdf_path, extracted_pages_root


def subimage_similarity_percentage(sub_image: cv2.Mat, image: cv2.Mat):
    template_match = cv2.matchTemplate(image, sub_image, cv2.TM_CCOEFF_NORMED)
    _, max_match_percentage, _, _ = cv2.minMaxLoc(template_match)

    return max_match_percentage


def match_pdf_file(root_dir: Path, sub_image: cv2.Mat):
    max_match_percentage, max_match_path = 0, ''

    for pdf_path, extracted_pages_root in generate_splitted_pdf(root_dir):
        for page_image_path in Path(extracted_pages_root).glob('*'):
            page_mat = cv2.imread(str(page_image_path), cv2.IMREAD_COLOR)
            current_match = subimage_similarity_percentage(sub_image, page_mat)

            if current_match > max_match_percentage:
                max_match_path = pdf_path
                max_match_percentage = current_match

            if current_match == 1:
                print(pdf_path, 'exactly matched')
                break

    return max_match_percentage, max_match_path


def parse_args():
    argument_parser = ArgumentParser('Program used to find pdf that contains given image')

    argument_parser.add_argument('-i', '--image', help='Path of image to search for', type=Path, required=True)
    argument_parser.add_argument('-d', '--directory', help='Path of root directory of pdf files to search in', type=Path, required=True)

    return argument_parser.parse_args()


def main():
    args = parse_args()

    image_to_search = cv2.imread(str(args.image), cv2.IMREAD_COLOR)
    max_match_percentage, max_match_path = match_pdf_file(args.directory, image_to_search)

    if max_match_path:
        print(
            f'Max matching pdf is {max_match_path.resolve()}',
            f'with {max_match_percentage}% matching')


if __name__ == '__main__':
    main()
