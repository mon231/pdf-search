#pragma once
#include "QueuesDefinitions.hpp"

#include <filesystem>
#include <poppler/cpp/poppler-page-renderer.h>

class PdfSearch final
{
public:
	explicit PdfSearch(
		const std::filesystem::path& image_to_search,
		const std::filesystem::path& pdfs_root);

public:
	void search();

private:
	void pdf_feeder_thread_entrypoint();
	void pdf_consumer_thread_entrypoint();
	void paths_feeder_thread_entrypoint();

private:
	const std::shared_ptr<cv::Mat> _image_to_search;
	const std::shared_ptr<poppler::page_renderer> _pdf_page_renderer;
	const std::filesystem::path _pdfs_root;
	const PdfImagesQueue _pdf_images_queue;
	const PdfPathsQueue _pdf_paths_queue;
};
