#include "PdfFeeder.hpp"
#include "PdfPagesIterator.hpp"

PdfFeeder::PdfFeeder(
	const std::filesystem::path& pdfs_root,
	const std::shared_ptr<poppler::page_renderer>& pdf_page_renderer,
	const PdfImagesQueue& pdf_pages_queue,
	const PdfPathsQueue& pdf_paths_queue) :
	_pdfs_root(pdfs_root),
	_pdf_page_renderer(pdf_page_renderer),
	_pdf_pages_queue(pdf_pages_queue),
	_pdf_paths_queue(pdf_paths_queue)
{
}

void PdfFeeder::feed_loop()
{
	while (true)
	{
		feed_pdf_file(_pdf_paths_queue->dequeue());
	}
}

void PdfFeeder::feed_pdf_file(const std::filesystem::path& pdf_path)
{
	for (
		PdfPagesIterator pdf_pages_iterator{ pdf_path.string(), _pdf_page_renderer };
		pdf_pages_iterator.has_more_pages();
		pdf_pages_iterator.iterate_to_next_page())
	{
		_pdf_pages_queue->enqueue(std::pair{
			pdf_pages_iterator.get_current_page(),
			pdf_pages_iterator.get_current_page_id() });
	}
}
