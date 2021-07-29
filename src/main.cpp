#include <chrono>
#include <ratio>
#include <string>
#include <ncurses/ncurses.h>
#include <iostream>
#include <filesystem>

#include "FileWatcher.hpp"
#include "KeyboardWatcher.hpp"
#include "ProgramHandler.hpp"
#include "config.h"

namespace fs = filesystem;

void show_help() {
	cout << "Usage: cmake_watcher [WATCH_PATH] [COMMAND_PATH] [COMMAND]" << endl;
	cout << "Watch a directory for changes and execute a custom command" << endl;
}

void on_update(ProgramHandler *handler, FileWatcher *fileWatcher) {
	if (handler->isFinished())
		handler->start();
	else
		handler->restart();
	fileWatcher->paused = false;
	fileWatcher->currentStatus = FileWatcher::UNKNOWN;
}


int main(int argc, char *argv[]) {

	if (argc < 4) {
		cout << "You should provide a directory to watch and a command to execute as well as a context path for the command!" << endl;
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

	//Ncurses init
	initscr();

	//Readonly terminal
	noecho();

	//Initializing Filewatcher and keyboardwatcher
	FileWatcher *fileWatcher = new FileWatcher(directory, chrono::milliseconds(100));
	KeyboardWatcher *keyboardWatcher = new KeyboardWatcher();
	ProgramHandler *programHandler = new ProgramHandler(arg3, arg2);

	//logging
	addstr("Starting file change watcher for directory ");
	printw(directory.c_str());
	addstr("\n");
	addstr("Press p to pause, r to rescan, q to quit\n");

	///We check if there are modified files and if the user pressed a key
	while (true) {
		switch (fileWatcher->currentStatus) {
			case FileWatcher::CREATED:
				addstr("File created");
				on_update(programHandler, fileWatcher);
				break;
			case FileWatcher::MODIFIED:
				addstr("File modified");
				on_update(programHandler, fileWatcher);
				break;
			case FileWatcher::DELETED:
				addstr("File deleted");
				on_update(programHandler, fileWatcher);
				break;
			default:
				break;
		}
		if (keyboardWatcher->hasKeyPressed('r')) {
			addstr("Rescanning...\n");
			fileWatcher->scan();
		}
		else if (keyboardWatcher->hasKeyPressed('q'))
			break;
	}
	fileWatcher->stop();
	endwin();
}