#include "PdfFile.hpp"

#include <hpdf.h>
#include <cstdint>
#include <filesystem>

PdfFile::PdfFile(const std::filesystem::path& file_path) :
    HpdfHandle()
{
    // TODO:
    // if (HPDF_OK != HPDF_LoadFile(_doc_handle, file_path.c_str()))
    // {
    //     std::cerr << "Failed to load the PDF document." << std::endl;
    //     HPDF_Free(pdf);
    //     return -1;
    // }
}

uint32_t PdfFile::get_pages_count() const
{
    return static_cast<uint32_t>(HPDF_GetPageCount(_doc_handle));
}
