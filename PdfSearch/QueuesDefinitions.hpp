#pragma once
#include "ThreadSafeQueue.hpp"

#include <memory>
#include <string>
#include <filesystem>
#include <opencv2/opencv.hpp>

using PdfPathsQueue = std::shared_ptr<ThreadSafeQueue<std::filesystem::path>>;
using PdfImagesQueue = std::shared_ptr<ThreadSafeQueue<std::pair<std::shared_ptr<cv::Mat>, std::string>>>;
