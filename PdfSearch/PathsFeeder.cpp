#include "PathsFeeder.hpp"

PathsFeeder::PathsFeeder(
	const std::filesystem::path& root_path,
	const std::string& file_extension,
	const PdfPathsQueue& paths_queue) :
	_root_path(root_path),
	_file_extension(file_extension),
	_paths_queue(paths_queue)
{
}

void PathsFeeder::feed_paths()
{
	for (const auto& fs_entry : std::filesystem::recursive_directory_iterator{ _root_path })
	{
		if (!fs_entry.is_regular_file())
		{
			continue;
		}

		const std::filesystem::path& entry_path = fs_entry.path();

		if (entry_path.extension() != "." + _file_extension)
		{
			continue;
		}

		_paths_queue->enqueue(entry_path);
	}
}
