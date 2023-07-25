#pragma once

#include <queue>
#include <memory>
#include <filesystem>
#include <opencv2/opencv.hpp>

class PdfSearch
{
public:
	explicit PdfSearch(
		const std::filesystem::path& image_to_search,
		const std::filesystem::path& pdfs_root);

public:
	void search();

private:
	void feeder_thread_entrypoint();
	void consumer_thread_entrypoint();

private:
	const cv::Mat _image_to_search;
	const std::filesystem::path _pdfs_root;
	const std::shared_ptr<std::queue<cv::Mat>> _pdf_images_queue;
};
