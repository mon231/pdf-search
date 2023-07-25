#include "PdfFeeder.hpp"
#include "PdfPagesIterator.hpp"

PdfFeeder::PdfFeeder(
	const std::filesystem::path& pdfs_root,
	const PdfImagesQueue& pdf_pages_queue) :
	_pdfs_root(pdfs_root),
	_renderer(std::make_shared<poppler::page_renderer>()),
	_pdf_pages_queue(pdf_pages_queue)
{
}

void PdfFeeder::feed_loop()
{
	for (const auto& fs_entry : std::filesystem::recursive_directory_iterator{ _pdfs_root })
	{
		if (!fs_entry.is_regular_file()) 
		{
			continue;
		}

		const std::filesystem::path& entry_path = fs_entry.path();

		if (entry_path.extension() != ".pdf")
		{
			continue;
		}

		feed_pdf_file(entry_path);
	}
}

void PdfFeeder::feed_pdf_file(const std::filesystem::path& pdf_path)
{
	for (
		PdfPagesIterator pdf_pages_iterator{ pdf_path.string(), _renderer };
		pdf_pages_iterator.has_more_pages();
		pdf_pages_iterator.iterate_to_next_page())
	{
		_pdf_pages_queue->enqueue(std::pair{
			pdf_pages_iterator.get_current_page(),
			pdf_pages_iterator.get_current_page_id() });
	}
}
