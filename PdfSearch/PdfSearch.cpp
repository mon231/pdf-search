#include "PdfSearch.hpp"

#include "PdfFeeder.hpp"
#include "PdfConsumer.hpp"
#include "PathsFeeder.hpp"

#include <list>
#include <thread>
#include <iostream>

PdfSearch::PdfSearch(
	const std::filesystem::path& image_to_search,
	const std::filesystem::path& pdfs_root) :
	_image_to_search(std::make_shared<cv::Mat>()),
	_pdfs_root(pdfs_root),
	_pdf_page_renderer(std::make_shared<poppler::page_renderer>()),
	_pdf_images_queue(std::make_shared<PdfImagesQueue::element_type>(100)),
	_pdf_paths_queue(std::make_shared<PdfPathsQueue::element_type>(100))
{
	_pdf_page_renderer->set_image_format(poppler::image::format_bgr24);

	cv::imread(image_to_search.string())
		.copyTo(*_image_to_search);
}

void PdfSearch::search()
{
	std::jthread paths_feeder(&PdfSearch::paths_feeder_thread_entrypoint, this);

	std::list<std::jthread> pdf_feeders;
	pdf_feeders.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
	pdf_feeders.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
	pdf_feeders.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);
	pdf_feeders.emplace_back(&PdfSearch::pdf_feeder_thread_entrypoint, this);

	std::list<std::jthread> pdf_consumers;
	pdf_consumers.emplace_back(&PdfSearch::pdf_consumer_thread_entrypoint, this);
	pdf_consumers.emplace_back(&PdfSearch::pdf_consumer_thread_entrypoint, this);
	pdf_consumers.emplace_back(&PdfSearch::pdf_consumer_thread_entrypoint, this);
	pdf_consumers.emplace_back(&PdfSearch::pdf_consumer_thread_entrypoint, this);

	for (auto& feeder_thread : pdf_feeders)
	{
		feeder_thread.join();
	}

	_pdf_images_queue->signal_quit();
}

void PdfSearch::pdf_feeder_thread_entrypoint()
{
	PdfFeeder pdf_feeder{ _pdfs_root, _pdf_page_renderer, _pdf_images_queue, _pdf_paths_queue };

	try
	{
		pdf_feeder.feed_loop();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void PdfSearch::pdf_consumer_thread_entrypoint()
{
	PdfConsumer pdf_consumer{ _image_to_search, _pdf_images_queue };

	try
	{
		pdf_consumer.consume_loop();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::cout << pdf_consumer.get_most_similar_description() << std::endl;
}

void PdfSearch::paths_feeder_thread_entrypoint()
{
	static constexpr const char* PDF_EXTENSION = "pdf";
	PathsFeeder paths_feeder{ _pdfs_root, PDF_EXTENSION, _pdf_paths_queue };

	try
	{
		paths_feeder.feed_paths();
		_pdf_paths_queue->signal_quit();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
