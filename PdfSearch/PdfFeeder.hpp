#pragma once
#include "QueuesDefinitions.hpp"

#include <memory>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <poppler/cpp/poppler-page-renderer.h>

class PdfFeeder
{
public:
	explicit PdfFeeder(
		const std::filesystem::path& pdfs_root,
		const std::shared_ptr<poppler::page_renderer>& pdf_page_renderer,
		const PdfImagesQueue& pdf_pages_queue,
		const PdfPathsQueue& pdf_paths_queue);

public:
	void feed_loop();

private:
	void feed_pdf_file(const std::filesystem::path& pdf_path);

private:
	const PdfPathsQueue _pdf_paths_queue;
	const PdfImagesQueue _pdf_pages_queue;
	const std::shared_ptr<poppler::page_renderer> _pdf_page_renderer;
	const std::filesystem::path _pdfs_root;
};
