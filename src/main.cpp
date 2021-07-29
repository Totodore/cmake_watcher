#include <chrono>
#include <ratio>
#include <string>
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "FileWatcher.hpp"
#include "ProgramHandler.hpp"
#include "config.h"

namespace fs = filesystem;

void show_help() {
	cout << "Usage: cmake_watcher [WATCH_PATH] [COMMAND_PATH] [COMMAND]" << endl;
	cout << "Watch a directory for changes and execute a custom command" << endl;
}

int main(int argc, char *argv[]) {

	if (argc < 4) {
		cout << "Error: You should provide a directory to watch and a command to execute as well as a context path for the command!" << endl;
		cout << endl;
		show_help();
		return 1;
	}


	fs::path directory;
	string arg1 = argv[1];
	string arg2 = argv[2];
	string arg3 = argv[3];
	for (int i = 4; i < argc; i++) {
		arg3 += " ";
		arg3 += argv[i];
	}

	if (arg1 == "help" || arg1 == "-h" || arg1 == "--help")
		show_help();
	else if (arg1[0] != '/')
		directory = fs::current_path().concat("/" + arg1);
	else
		directory = fs::current_path().concat(arg1);

	if (!fs::exists(directory)) {
		cout << "The provided directory does not exists: ";
		cout << directory << endl;
		return 1;
	}

	//Initializing Filewatcher and keyboardwatcher
	FileWatcher *fileWatcher = new FileWatcher(directory, chrono::milliseconds(100));
	ProgramHandler *programHandler = new ProgramHandler(arg3, arg2);

	//logging
	spdlog::info("Starting file change watcher for directory " + directory.string());

	bool endedLog = false;
	bool fileAction = false;
	///We check if there are modified files and if the user pressed a key
	while (true) {
		switch (fileWatcher->currentStatus) {
			case FileWatcher::CREATED:
				spdlog::info("File created");
				fileAction = true;
				break;
			case FileWatcher::MODIFIED:
				spdlog::info("File modified");
				fileAction = true;
				break;
			case FileWatcher::DELETED:
				spdlog::info("File deleted");
				fileAction = true;
				break;
			default:
				break;
		}
		if (programHandler->isFinished() && !programHandler->isErrored() && !endedLog) {
			spdlog::info("Program execution ended, waiting for file change...");
			endedLog = true;
		} else if (programHandler->isFinished() && programHandler->isErrored() && !endedLog) {
			spdlog::error("Error while executing program, waiting for file change...");
			endedLog = true;
		} else if (!programHandler->isFinished() && endedLog)
			endedLog = false;
	
		if (fileAction) {
			programHandler->restart();
			fileWatcher->paused = false;
			fileWatcher->currentStatus = FileWatcher::UNKNOWN;
			fileAction = false;
		}
	}
	fileWatcher->stop();
	programHandler->stop();
}