#include "PdfPagesIterator.hpp"

#include <memory>
#include <string>
#include <opencv2/opencv.hpp>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page-renderer.h>

PdfPagesIterator::PdfPagesIterator(
    const std::string& pdf_path,
    const std::shared_ptr<poppler::page_renderer>& renderer):
	_current_page_index(),
    _pdf_document(poppler::document::load_from_file(pdf_path)),
    _pdf_page_renderer(renderer),
	_pdf_path(pdf_path)
{
}

void PdfPagesIterator::iterate_to_next_page()
{
    ++_current_page_index;
}

bool PdfPagesIterator::has_more_pages() const
{
    return static_cast<size_t>(_pdf_document->pages()) != _current_page_index;
}

std::shared_ptr<cv::Mat> PdfPagesIterator::get_current_page() const
{
	std::unique_ptr<poppler::page> current_page{ _pdf_document->create_page(_current_page_index) };

	static constexpr double PAGE_DPI = 200;
	poppler::image current_page_image = _pdf_page_renderer->render_page(current_page.get(), PAGE_DPI, PAGE_DPI);

	cv::Mat notOwningImageMat{
		current_page_image.height(),
		current_page_image.width(),
		CV_8UC3,
		current_page_image.data(),
		static_cast<size_t>(current_page_image.bytes_per_row()) };

	std::shared_ptr<cv::Mat> resultSharedMat = std::make_shared<cv::Mat>();
	notOwningImageMat.copyTo(*resultSharedMat);

	return resultSharedMat;
}

std::string PdfPagesIterator::get_current_page_id() const
{
	return _pdf_path + ", at page " + std::to_string(_current_page_index);
}
