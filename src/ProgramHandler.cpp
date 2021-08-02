#include "ProgramHandler.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/process.hpp>
#include <cstdio>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

namespace tree = boost::property_tree;


ProgramHandler::ProgramHandler(fs::path contextPath) : contextPath(contextPath) {
  basePath = fs::current_path();

	tree::ptree root;
	tree::read_json("watcher.conf.json", root);
  for (tree::ptree::value_type &command : root.get_child("exec")) {
    commands.push_back(command.second.data());
	}
}

void ProgramHandler::start() {
  spdlog::debug("Current dir " + fs::current_path().string());
  if (chdir(contextPath.c_str()) != 0) {
    spdlog::error("Could not cd to the execution directory");
    return;
  }
  for (auto &command : commands) {
    if ((children.size() > 0 && children.back().exit_code() != 0) || terminate) {
			terminate = false;
      break;
    }
    spdlog::debug("Executing program: {}", command);
    try {
      children.push_back(
				process::child(command, process::std_out > stdout, process::std_err > stderr, process::std_in < stdin)
			);
      spdlog::debug("Process started {}", command);
      children.back().wait();
			if (isErrored())
				break;
    } catch (const process::process_error &e) {
      spdlog::error("Could not execute program: {} ({})", command, e.code().message());
      break;
    }
  }
  baseChdir();
}

void ProgramHandler::restart() {
  spdlog::debug("finished {}", isFinished());
  if (!isFinished())
    stop();
  start();
}

bool ProgramHandler::isErrored() {
  return children.size() > 0 && children.back().exit_code() != 0;
}

bool ProgramHandler::isFinished() {
  return isErrored() || children.size() == commands.size();
}

void ProgramHandler::baseChdir() {
  if (chdir(basePath.c_str()) != 0)
    spdlog::error("Could not cd to the base directory");
}

void ProgramHandler::stop() {
  spdlog::debug("stopping program");
  children.back().terminate();
	terminate = true;
  baseChdir();
}
