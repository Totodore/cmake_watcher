#include <list>
#include <boost/thread.hpp>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class ProgramHandler {

public:
	int errorCode;
	ProgramHandler(string, fs::path);
	~ProgramHandler();
	
	void start();
	void stop();
	void restart();
	bool isErrored();
	bool isFinished();

private:
	string command;
	fs::path contextPath;
	fs::path basePath;
	boost::thread thread;
	bool finished = false;
	void run();
	void baseChdir();
};