#include "ProgramHandler.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ncurses/ncurses.h>
#include <thread>
#include <unistd.h>

ProgramHandler::ProgramHandler(string command, fs::path contextPath) : command(command), contextPath(contextPath) { }

void ProgramHandler::start() {
  this->thread = boost::thread(boost::bind(&ProgramHandler::run, this));
}

void ProgramHandler::run() {
	if (chdir(contextPath.c_str()) != 0)
		return;
	this->errorCode = system(this->command.c_str());
	this->finished = true;
}

void ProgramHandler::restart() {
	this->stop();
	this->finished = false;
	this->errorCode = 0;
	this->start();
}

bool ProgramHandler::isErrored() {
	return this->errorCode > 0;
}

bool ProgramHandler::isFinished() {
	return this->finished;
}

void ProgramHandler::stop() { this->thread.interrupt(); }

