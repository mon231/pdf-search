#include "PdfConsumer.hpp"
#include <iostream>

PdfConsumer::PdfConsumer(
	const cv::Mat& searched_image,
	const std::shared_ptr<std::queue<cv::Mat>> pages_queue) :
	_searched_image(searched_image),
	_pages_queue(pages_queue),
	_most_similar_pdf_similarity(),
	_most_similar_pdf_path()
{}

void PdfConsumer::consume_loop()
{
	while (true)
	{
		consume_one_page(_pages_queue->back());
		_pages_queue->pop();
	}
}

void PdfConsumer::consume_one_page(const cv::Mat& page)
{
	const double simi = get_similarity(page, _searched_image);
	
	if (simi == 1)
	{
		// TODO: cout path
	}

	if (simi > _most_similar_pdf_similarity)
	{
		// TODO: update path
		_most_similar_pdf_similarity = simi;
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
