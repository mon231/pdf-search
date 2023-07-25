#include "PdfSearch.hpp"

#include "PdfFeeder.hpp"
#include "PdfConsumer.hpp"

#include <thread>

PdfSearch::PdfSearch(
	const std::filesystem::path& image_to_search, 
	const std::filesystem::path& pdfs_root):
	_image_to_search(cv::imread(image_to_search.string())),
	_pdfs_root(pdfs_root),
	_pdf_images_queue(std::make_shared<std::queue<cv::Mat>>())
{}

void PdfSearch::search()
{
	std::thread feeder_thread{ &PdfSearch::feeder_thread_entrypoint, this };
	std::thread consumer_thread{ &PdfSearch::consumer_thread_entrypoint, this };

	feeder_thread.join();
	consumer_thread.join();
}

void PdfSearch::feeder_thread_entrypoint()
{
	PdfFeeder pdf_feeder{ _pdfs_root, _pdf_images_queue };
	pdf_feeder.feed_loop();
}

void PdfSearch::consumer_thread_entrypoint()
{
	PdfConsumer pdf_consumer{ _image_to_search, _pdf_images_queue };
	pdf_consumer.consume_loop();
}
