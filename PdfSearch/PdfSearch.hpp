#pragma once

#include "PdfImagesQueue.hpp"
#include <filesystem>

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
	const PdfImagesQueue _pdf_images_queue;
};
