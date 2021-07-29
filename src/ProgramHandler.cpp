#include "ProgramHandler.hpp"

#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <filesystem>

ProgramHandler::ProgramHandler(string command, fs::path contextPath) : command(command), contextPath(contextPath) { 
	this->basePath = fs::current_path();
}

void ProgramHandler::start() {
  this->thread = boost::thread(boost::bind(&ProgramHandler::run, this));
}

void ProgramHandler::run() { 
	try {
		spdlog::debug("Current dir " + fs::current_path().string());
	if (chdir(contextPath.c_str()) != 0) {
		spdlog::error("Could not cd to the execution directory");
		return;
	}
	spdlog::debug("Executing " + this->command);
	this->errorCode = system(this->command.c_str());
	this->baseChdir();
	this->finished = true;
	} catch(boost::thread_interrupted&) {
		this->finished = true;
		spdlog::debug("thread thread_interrupted");
	}
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

void ProgramHandler::baseChdir() {
	if (chdir(basePath.c_str()) != 0)
		spdlog::error("Could not cd to the base directory");
}

void ProgramHandler::stop() { 
	spdlog::debug("test");
	this->thread.interrupt();
	this->baseChdir();
}

