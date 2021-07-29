#include <chrono>
#include <filesystem>
#include <ratio>
#include <string>
#include <unordered_map>
#include <boost/thread.hpp>

using namespace std;


class FileWatcher {

public:
	enum FileStatus { CREATED, MODIFIED, DELETED, UNKNOWN };
	
	FileStatus currentStatus = FileStatus::UNKNOWN;
	bool paused = false;

  FileWatcher(string, chrono::duration<long int, milli>);

  FileWatcher* start();
  void stop();

private:
  unordered_map<string, filesystem::file_time_type> paths;
  string path;
	boost::thread thread;
  chrono::duration<long int, milli> delay;

  void watch();
};