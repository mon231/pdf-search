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

int main()
{
	static constexpr const char* x = "C:/Users/avart/Desktop/ComputerSciense/infi2/dict.pdf";
	std::unique_ptr<poppler::document> doc{ poppler::document::load_from_file(x) };

	for (uint32_t i = 0; i < doc->pages(); ++i)
	{
		std::unique_ptr<poppler::page> pg{ doc->create_page(i) };
		// TODO: pg to cv::Mat
	}

	return EXIT_SUCCESS;
}
