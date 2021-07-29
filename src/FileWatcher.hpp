#include <chrono>
#include <filesystem>
#include <ratio>
#include <string>
#include <unordered_map>
#include <boost/thread.hpp>

using namespace std;
namespace fs = filesystem;

class FileWatcher {

public:
	enum FileStatus { CREATED, MODIFIED, DELETED, UNKNOWN };
	
	FileStatus currentStatus = FileStatus::UNKNOWN;
	bool paused = false;
	fs::path currentActionPath;

  FileWatcher(string, chrono::duration<long int, milli>);

  void stop();
	void scan();

private:
  unordered_map<string, fs::file_time_type> paths;
  fs::path watchingPath;
	boost::thread thread;
  chrono::duration<long int, milli> delay;
	
  void watch();
};