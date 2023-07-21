#pragma once
#include <hpdf.h>

class HpdfHandle
{
public:
    HpdfHandle();
    ~HpdfHandle();

protected:
    HPDF_Doc _doc_handle;
};
