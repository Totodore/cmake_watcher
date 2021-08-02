#include "boost/process/detail/child_decl.hpp"
#include <list>
#include <filesystem>
#include <thread>
#include <unistd.h>
#include <boost/process.hpp>

using namespace std;
namespace fs = filesystem;
namespace process = boost::process;

class ProgramHandler {

public:
	int errorCode;
	ProgramHandler(fs::path);

	
	void start();
	void stop();
	void restart();
	bool isErrored();
	bool isFinished();

private:
	fs::path contextPath;
	fs::path basePath;
	vector<process::child> children;
	vector<string> commands;
	bool terminate = false;

	void run();
	void spawn();
	void baseChdir();
	void sigquitHandler(int);
};