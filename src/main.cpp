#include <chrono>
#include <ratio>
#include <string>
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "FileWatcher.hpp"
#include "ProgramHandler.hpp"
#include "config.h"
#include "spdlog/common.h"

namespace fs = filesystem;

/**
 * @brief Function that display basic help and explanations on the program
 * 
 */
void show_help() {
	cout << "Usage: cmake_watcher [WATCH_PATH] [COMMAND_PATH] [COMMAND]" << endl;
	cout << "Watch a directory for changes and execute a custom command" << endl;
}

int main(int argc, char *argv[]) {

	/**
	 * @brief If not all the parameters are specified we display an error and show help
	 * 
	 */
	if (argc < 4) {
		cout << "Error: You should provide a directory to watch and a command to execute as well as a context path for the command!" << endl;
		cout << endl;
		show_help();
		return 1;
	}


	/**
	 * @brief We set all the parameters given to the program
	 * 
	 */
	fs::path directory;
	string sourcePath = argv[1];
	string commandPath = argv[2];
	string command = argv[3];
	for (int i = 4; i < argc; i++) {
		command += " ";
		command += argv[i];
	}

	if (sourcePath == "help" || sourcePath == "-h" || sourcePath == "--help")
		show_help();
	else if (sourcePath[0] != '/')
		directory = fs::current_path().concat("/" + sourcePath);
	else
		directory = fs::current_path().concat(sourcePath);

	if (!fs::exists(directory)) {
		cout << "The provided directory does not exists: ";
		cout << directory << endl;
		return 1;
	}

	spdlog::set_level(PROD_MODE ? spdlog::level::info : spdlog::level::debug);

	//Initializing Filewatcher and ProgramHandler
	FileWatcher *fileWatcher = new FileWatcher(directory, chrono::milliseconds(100));
	ProgramHandler *programHandler = new ProgramHandler(command, commandPath);

	//logging
	spdlog::info("Starting file change watcher for directory " + directory.string());

	bool endedLog = false;
	bool fileAction = false;
	/**
	 * @brief Infinite loop that check if file are updated
	 * 
	 */
	while (true) {
		switch (fileWatcher->currentStatus) {
			case FileWatcher::CREATED:
				spdlog::info("File created");
				fileAction = true;
				break;
			case FileWatcher::MODIFIED:
				spdlog::info("Modified file: " + fileWatcher->currentActionPath.string());
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