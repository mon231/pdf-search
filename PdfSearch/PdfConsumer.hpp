#pragma once

#include <queue>
#include <memory>
#include <opencv2/opencv.hpp>

class PdfConsumer
{
public:
	explicit PdfConsumer(
		const cv::Mat& searched_image,
		const std::shared_ptr<std::queue<cv::Mat>> pdf_pages_queue);

public:
	void consume_loop();

private:
	void consume_one_page(const cv::Mat& page);
	
private:
	static double get_similarity(const cv::Mat& page, const cv::Mat& subimage);

private:
	double _most_similar_pdf_similarity;
	std::string _most_similar_pdf_path;
	const cv::Mat _searched_image;
	const std::shared_ptr<std::queue<cv::Mat>> _pdf_pages_queue;
};
