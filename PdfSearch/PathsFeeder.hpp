#pragma once

#include "ThreadSafeQueue.hpp"
#include "QueuesDefinitions.hpp"

#include <string>
#include <filesystem>

class PathsFeeder
{
public:
	explicit PathsFeeder(
		const std::filesystem::path& root_path,
		const std::string& file_extension,
		const PdfPathsQueue& paths_queue);

	void feed_paths();

private:
	const std::string _file_extension;
	const std::filesystem::path _root_path;
	const PdfPathsQueue _paths_queue;
};
