#pragma once
#include "HpdfHandle.hpp"

#include <cstdint>
#include <filesystem>

class PdfFile final : private HpdfHandle
{
public:
    PdfFile(const std::filesystem::path& file_path);

public:
    uint32_t get_pages_count() const;
};

//     HPDF_Doc pdf = HPDF_New(NULL, NULL);

//     // Load the PDF document
//     if (HPDF_OK != HPDF_LoadFile(pdf, "input.pdf")) {
//         std::cerr << "Failed to load the PDF document." << std::endl;
//         HPDF_Free(pdf);
//         return -1;
//     }

//     // Get the total number of pages in the PDF document
//     int total_pages = HPDF_GetPageCount(pdf);

//     // Iterate over each page and convert it to an OpenCV cv::Mat object
//     for (int i = 0; i < total_pages; ++i) {
//         // Get the current page
//         HPDF_Page page = HPDF_GetPage(pdf, i);

//         // Get the width and height of the page
//         int page_width = HPDF_Page_GetWidth(page);
//         int page_height = HPDF_Page_GetHeight(page);

//         // Create an OpenCV cv::Mat with the same size as the page
//         cv::Mat cv_image(page_height, page_width, CV_8UC3, cv::Scalar(255, 255, 255));

//         // Render the page content to the cv::Mat object
//         HPDF_Image hpdf_image = HPDF_Page_GetImage(page, NULL, NULL);
//         if (hpdf_image) {
//             HPDF_UINT width, height;
//             HPDF_Image_GetSize(hpdf_image, &width, &height);

//             std::vector<unsigned char> img_data(width * height * 3);
//             HPDF_Image_GetRGB(hpdf_image, &img_data[0], HPDF_RGB, 0, 0, width, height);

//             cv_image.data = img_data.data();
//         }

//         // Process the OpenCV cv::Mat object as needed

//         // Save the cv::Mat object as an image (optional)
//         std::string output_filename = "page_" + std::to_string(i + 1) + ".png";
//         cv::imwrite(output_filename, cv_image);
//     }

//     // Free resources and clean up
//     HPDF_Free(pdf);

