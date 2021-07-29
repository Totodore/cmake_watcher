#include <list>
#include <boost/thread.hpp>

using namespace std;

class KeyboardWatcher {

public:
	KeyboardWatcher();
	~KeyboardWatcher();
	
	void stop();
	bool hasKeyPressed(const char key);
	
private:
	list<char> *pressedKeys = new list<char>();
	boost::thread thread;
	void watch();
};