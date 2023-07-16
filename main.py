import cv2
from pathlib import Path
from PyPDF2 import PdfReader
from argparse import ArgumentParser


def subimage_similarity_percentage(sub_image: cv2.Mat, image: cv2.Mat):
    template_match = cv2.matchTemplate(image, sub_image, cv2.TM_CCOEFF_NORMED)
    _, max_match_percentage, _, _ = cv2.minMaxLoc(template_match)

    return max_match_percentage


def match_pdf_file(root_dir: Path, sub_image: cv2.Mat):
    max_match_percentage, max_match_path, max_match_page_index = 0, None, 0

    for pdf_path in root_dir.rglob('*.pdf'):
        pdf_reader = PdfReader(pdf_path)

        for page_index, pdf_page in enumerate(pdf_reader).pages:
            # TODO: find matching percentage
            page_mat: cv2.Mat = pdf_page
            current_match = subimage_similarity_percentage(sub_image, page_mat)

            if current_match > max_match_percentage:
                max_match_path = pdf_path
                max_match_page_index = page_index
                max_match_percentage = current_match

    return max_match_percentage, max_match_path, max_match_page_index


def parse_args():
    argument_parser = ArgumentParser('Program used to find pdf that contains given image')

    argument_parser.add_argument('-i', '--image', help='Path of image to search for', type=Path)
    argument_parser.add_argument('-d', '--directory', help='Path of root directory of pdf files to search in', type=Path)

    return argument_parser.parse_args()


def main():
    args = parse_args()
    image_to_search = cv2.imread(args.image, cv2.IMREAD_COLOR)
    max_match_percentage, max_match_path, max_match_page_index = match_pdf_file(args.directory, image_to_search)

    if max_match_path:
        print(
            f'Max matching pdf is {max_match_path.resolve()} '
            f'at page {max_match_page_index + 1}',
            f'with {max_match_percentage}% matching')


if __name__ == '__main__':
    main()
