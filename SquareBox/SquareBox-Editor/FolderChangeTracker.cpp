#include "FolderChangeTracker.h"

void FolderChangeTracker::operator()(std::string path_, std::function<void(std::string)> onModify_)
{
	bool commonTest = false;
	bool useGeneric = false;


	UpdateListener * ul = new UpdateListener();
	ul->init(onModify_);
	/// create the file watcher object
	efsw::FileWatcher fileWatcher(useGeneric);

	fileWatcher.followSymlinks(false);
	fileWatcher.allowOutOfScopeLinks(false);

	if (efsw::FileSystem::isDirectory(path_))
	{
		efsw::WatchID err;

		if ((err = fileWatcher.addWatch(path_, ul, true)) > 0)
		{
			fileWatcher.watch();

			std::cout << "Watching directory: " << path_.c_str() << std::endl;

			if (useGeneric)
			{
				std::cout << "Using generic backend watcher" << std::endl;
			}
		}
		else
		{
			std::cout << "Error trying to watch directory: " << path_.c_str() << std::endl;
			std::cout << efsw::Errors::Log::getLastErrorLog().c_str() << std::endl;
		}
	}
	else {
		std::cout << "Invalid Path " << std::endl;
	}
	while (true)
	{
		efsw::System::sleep(100);
	}
}