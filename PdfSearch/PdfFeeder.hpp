#pragma once

#include <queue>
#include <memory>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <poppler/cpp/poppler-page-renderer.h>

class PdfFeeder
{
public:
	explicit PdfFeeder(
		const std::filesystem::path& pdfs_root,
		const std::shared_ptr<std::queue<cv::Mat>> pdf_pages_queue);

public:
	void feed_loop();

private:
	void feed_pdf_file(const std::filesystem::path& pdf_path);

private:
	const std::shared_ptr<std::queue<cv::Mat>> _pdf_pages_queue;
	const std::shared_ptr<poppler::page_renderer> _renderer;
	const std::filesystem::path _pdfs_root;
};
