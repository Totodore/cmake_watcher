#include <chrono>
#include <ratio>
#include <string>
#include <ncurses/ncurses.h>
#include <iostream>
#include <filesystem>

#include "FileWatcher.hpp"
#include "KeyboardWatcher.hpp"
#include "config.h"

namespace fs = filesystem;

int main(int argc, char *argv[]) {

	if (argc < 2) {
		cout << "You should provide a directory to watch!" << endl;
		return 1;
	}
	fs::path directory;
	if (argv[1][0] != '/')
		directory = fs::current_path().concat("/" + string(argv[1]));
	else
		directory = fs::current_path().concat(string(argv[1]));
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
				fileWatcher->paused = false;
				fileWatcher->currentStatus = FileWatcher::UNKNOWN;
				break;
			case FileWatcher::MODIFIED:
				addstr("File modified");
				fileWatcher->paused = false;
				fileWatcher->currentStatus = FileWatcher::UNKNOWN;
				break;
			case FileWatcher::DELETED:
				addstr("File deleted");
				fileWatcher->paused = false;
				fileWatcher->currentStatus = FileWatcher::UNKNOWN;
				break;
			default:
				break;
		}
		if (keyboardWatcher->hasKeyPressed('p'))
			addstr("File Watcher is in pause\n");
		else if (keyboardWatcher->hasKeyPressed('r'))
			addstr("Rescanning...\n");
		else if (keyboardWatcher->hasKeyPressed('q'))
			break;
	}
	fileWatcher->stop();
	endwin();
}