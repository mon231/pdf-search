#pragma once

#include <memory>
#include <string>
#include <opencv2/opencv.hpp>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page-renderer.h>

class PdfPagesIterator
{
public:
	explicit PdfPagesIterator(
		const std::string& pdf_path, 
		const std::shared_ptr<poppler::page_renderer>& renderer);

	void iterate_to_next_page();
	bool has_more_pages() const;

	std::string get_current_page_id() const;
	std::shared_ptr<cv::Mat> get_current_page() const;

private:
	size_t _current_page_index;
	const std::string _pdf_path;
	const std::unique_ptr<poppler::document> _pdf_document;
	const std::shared_ptr<poppler::page_renderer> _pdf_page_renderer;
};
