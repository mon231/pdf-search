#include "PdfConsumer.hpp"
#include <iostream>

PdfConsumer::PdfConsumer(
	const cv::Mat& searched_image,
	const PdfImagesQueue& pdf_pages_queue) :
	_searched_image(searched_image),
	_pdf_pages_queue(pdf_pages_queue),
	_most_similar_page_similarity(),
	_most_similar_page_id()
{}

void PdfConsumer::consume_loop()
{
	while (true)
	{
		auto [page_image, page_id] = _pdf_pages_queue->dequeue();
		consume_one_page(page_image, page_id);
	}
}

void PdfConsumer::consume_one_page(const cv::Mat& page, const std::string& page_id)
{
	const double similarity = get_similarity(page, _searched_image);
	
	if (similarity == 1.0)
	{
		std::cout << page_id << std::endl;
	}

	if (similarity > _most_similar_page_similarity)
	{
		_most_similar_page_id = page_id;
		_most_similar_page_similarity = similarity;
	}
}

double PdfConsumer::get_similarity(const cv::Mat& page, const cv::Mat& subimage)
{
	cv::Mat restempl{};
	cv::matchTemplate(page, subimage, restempl, cv::TM_CCOEFF_NORMED);

	double max_val = 0;
	cv::minMaxIdx(page, nullptr, &max_val, nullptr, nullptr, restempl);

	return max_val;
}
