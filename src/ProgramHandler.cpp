#include "ProgramHandler.hpp"

#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <filesystem>

ProgramHandler::ProgramHandler(string command, fs::path contextPath) : command(command), contextPath(contextPath) { }

void ProgramHandler::start() {
  this->thread = boost::thread(boost::bind(&ProgramHandler::run, this));
}

void ProgramHandler::run() {
	auto currentPath = fs::current_path();
	if (chdir(contextPath.c_str()) != 0) {
		spdlog::error("Could not cd to the execution directory");
		return;
	}
	this->errorCode = system(this->command.c_str());
	if (chdir(currentPath.c_str()) != 0) {
		spdlog::error("Could not cd to the base directory");
	}
	this->finished = true;
}

void ProgramHandler::restart() {
	if (!this->finished)
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

