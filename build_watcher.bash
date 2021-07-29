#!/bin/bash

build() {
	echo "Building..."
	cd build/
	cmake -DCMAKE_BUILD_TYPE=Release ..
	cmake --build .
	./bin/cmake_watcher ..
}

build