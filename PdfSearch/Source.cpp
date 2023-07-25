#include <memory>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <poppler/cpp/poppler-toc.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-destination.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-page-transition.h>

double get_similarity(const cv::Mat& page, const cv::Mat& subimage)
{
	cv::Mat restempl{};
	cv::matchTemplate(page, subimage, restempl, cv::TM_CCOEFF_NORMED);

	double max_val = 0;
	cv::minMaxIdx(page, nullptr, &max_val, nullptr, nullptr, restempl);

	return max_val;
}

int main()
{
	static constexpr const char* PDF_FILE_PATH = "<PDF PATH>";
	std::unique_ptr<poppler::document> doc{ poppler::document::load_from_file(PDF_FILE_PATH) };

	poppler::page_renderer renderer;
	renderer.set_image_format(poppler::image::format_bgr24);

	for (uint32_t i = 0; i < doc->pages(); ++i)
	{
		std::unique_ptr<poppler::page> current_page{ doc->create_page(i) };

		static constexpr double DPI = 200;
		poppler::image current_page_image = renderer.render_page(current_page.get(), DPI, DPI);

		cv::Mat current_page_mat{
			current_page_image.height(),
			current_page_image.width(),
			CV_8UC3,
			current_page_image.data(),
			(size_t)current_page_image.bytes_per_row()};
	}

	return EXIT_SUCCESS;
}
