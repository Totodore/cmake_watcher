#include "KeyboardWatcher.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ncurses/ncurses.h>
#include <thread>

KeyboardWatcher::KeyboardWatcher() {}

KeyboardWatcher::~KeyboardWatcher() {
  this->stop();
  delete this->pressedKeys;
}

KeyboardWatcher* KeyboardWatcher::start() {
  this->thread = boost::thread(boost::bind(&KeyboardWatcher::watch, this));
	return this;
}

void KeyboardWatcher::stop() { this->thread.interrupt(); }

void KeyboardWatcher::watch() {
  try {
    while (true) {
			this->pressedKeys->push_back(getch());
			this_thread::sleep_for(chrono::duration<int, milli>(20));
		}
  } catch (boost::thread_interrupted &) {
  }
}

bool KeyboardWatcher::hasKeyPressed(const char key) {
	for (auto &pressedKey : *this->pressedKeys) {
		if (pressedKey == key) {
			this->pressedKeys->remove(pressedKey);
			return true;
		}
	}
	return false;
}