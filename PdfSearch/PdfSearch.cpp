#include "PdfSearch.hpp"

#include "PdfFeeder.hpp"
#include "PdfConsumer.hpp"
#include "PathsFeeder.hpp"

#include <list>
#include <thread>

PdfSearch::PdfSearch(
	const std::filesystem::path& image_to_search,
	const std::filesystem::path& pdfs_root) :
	_image_to_search(cv::imread(image_to_search.string())),
	_pdfs_root(pdfs_root),
	_pdf_images_queue(std::make_shared<PdfImagesQueue::element_type>()),
	_pdf_paths_queue(std::make_shared<PdfPathsQueue::element_type>())
{}

void PdfSearch::search()
{
	std::list<std::jthread> lst;

	lst.emplace_back(&PdfSearch::paths_feeder_thread_entrypoint, this);
	lst.emplace_back(&PdfSearch::pdf_consumer_thread_entrypoint, this);

	lst.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
	lst.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
	lst.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
	lst.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
}

void PdfSearch::pdf_feeder_thread_entrypoint()
{
	PdfFeeder pdf_feeder{ _pdfs_root, _pdf_images_queue, _pdf_paths_queue };
	pdf_feeder.feed_loop();
}

void PdfSearch::pdf_consumer_thread_entrypoint()
{
	PdfConsumer pdf_consumer{ _image_to_search, _pdf_images_queue };
	pdf_consumer.consume_loop();
}

void PdfSearch::paths_feeder_thread_entrypoint()
{
	static constexpr const char* PDF_EXTENSION = "pdf";
	PathsFeeder paths_feeder{ _pdfs_root, PDF_EXTENSION, _pdf_paths_queue };

	paths_feeder.feed_paths();
	_pdf_paths_queue->signal_quit();
}
