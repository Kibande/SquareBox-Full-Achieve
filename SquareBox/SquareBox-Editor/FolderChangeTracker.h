#pragma once
#include <efsw/efsw.hpp>
#include <efsw/System.hpp>
#include <efsw/FileSystem.hpp>
#include <iostream>
#include <thread>
#include<functional>
/// Processes a file action
class UpdateListener : public efsw::FileWatchListener
{
public:
	UpdateListener() {}
	void init(std::function<void(std::string)> onModify) {
		m_onModify_Func = onModify;
	}
	std::string getActionName(efsw::Action action)
	{
		switch (action)
		{
		case efsw::Actions::Add:		return "Add";
		case efsw::Actions::Modified:	return "Modified";
		case efsw::Actions::Delete:		return "Delete";
		case efsw::Actions::Moved:		return "Moved";
		default:						return "Bad Action";
		}
	}

	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "")
	{
		if (action == efsw::Actions::Modified) {
			m_onModify_Func(dir+filename);
		}
		else {
			std::string error_sentence = "Error " + dir + filename + " registered a " + getActionName(action);
			std::cout << error_sentence << std::endl;
		}
	}

private:
	std::function<void(std::string)> m_onModify_Func;
};


class FolderChangeTracker
{
public:
	void operator()(std::string path_, std::function<void(std::string)> onModify_);
private:

};



