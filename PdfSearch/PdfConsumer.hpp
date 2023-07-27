#pragma once
#include "QueuesDefinitions.hpp"

#include <memory>
#include <opencv2/opencv.hpp>

class PdfConsumer final
{
public:
	explicit PdfConsumer(
		const std::shared_ptr<cv::Mat>& searched_image,
		const PdfImagesQueue& pdf_pages_queue);

public:
	void consume_loop();
	std::string get_most_similar_description() const;

private:
	void consume_one_page(const cv::Mat& page, const std::string& page_id);

private:
	static double get_similarity(const cv::Mat& page, const cv::Mat& subimage);

private:
	double _most_similar_page_similarity;
	std::string _most_similar_page_id;
	const std::shared_ptr<cv::Mat> _searched_image;
	const PdfImagesQueue _pdf_pages_queue;
};
