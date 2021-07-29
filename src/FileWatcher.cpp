#include "FileWatcher.hpp"

#include <filesystem>
#include <ratio>
#include <string>
#include <chrono>
#include <unordered_map>
#include <boost/thread.hpp>
#include <thread>

using namespace std;

FileWatcher::FileWatcher(string path, chrono::duration<long int, milli> delay) : path(path), delay(delay) {
	this->scan();
	this->thread = boost::thread(boost::bind(&FileWatcher::watch, this));
}


void FileWatcher::scan() { 
	this->paths.clear();
	for(auto &file : filesystem::recursive_directory_iterator(path)) {
		this->paths[file.path().string()] = filesystem::last_write_time(file);
	}
}

void FileWatcher::stop() {
	this->paused = false;
	this->thread.interrupt();
	this->paths.clear();
}

void FileWatcher::watch() {
	try {
		bool needScan = false;
		while(true) {
			if (this->paused)
				continue;
			if (needScan) {
				this->scan();
				needScan = false;
			}

			int i = 0;			
			for(auto &file : filesystem::recursive_directory_iterator(path)) {
				if (this->paths[file.path().string()] != filesystem::last_write_time(file)) {
					this->paths[file.path().string()] = filesystem::last_write_time(file);
					this->currentStatus = FileStatus::MODIFIED;
					this->paused = true;
				}
				i++;
			}
			if (i > this->paths.size()) {
				this->currentStatus = FileStatus::CREATED;
				this->paused = true;
				needScan = true;
			}
			else if (i < this->paths.size()) {
				this->currentStatus = FileStatus::DELETED;
				this->paused = true;
				needScan = true;
			}
			this_thread::sleep_for(this->delay);
		}
	} catch(boost::thread_interrupted&) {}
}