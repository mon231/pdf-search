#pragma once
#include "ThreadSafeQueue.hpp"

#include <memory>
#include <string>
#include <opencv2/opencv.hpp>

using PdfImagesQueue = std::shared_ptr<ThreadSafeQueue<std::pair<cv::Mat, std::string>>>;
