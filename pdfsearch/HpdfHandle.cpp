#include "HpdfHandle.hpp"
#include <exception>

HpdfHandle::HpdfHandle() :
    _doc_handle(HPDF_New(nullptr, nullptr))
{
    if (_doc_handle == nullptr)
    {
        throw std::exception("Error creating hpdf resource");
    }
}

HpdfHandle::~HpdfHandle()
{
    try
    {
        HPDF_Free(_doc_handle);
    }
    catch(...)
    {
    }
}
