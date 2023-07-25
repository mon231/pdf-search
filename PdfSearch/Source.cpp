#include "PdfSearch.hpp"

int main()
{
	static constexpr const char* SEARCHED_IMAGE = "<SEARCHED IMAGE>";
	static constexpr const char* PDFS_ROOT = "<PDFS ROOT>";

	PdfSearch pdf_search{ SEARCHED_IMAGE, PDFS_ROOT };
	pdf_search.search();

	return EXIT_SUCCESS;
}
