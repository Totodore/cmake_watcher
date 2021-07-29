#!/bin/bash

build() {
	echo "Building..."
	cd build/
	cmake -DCMAKE_BUILD_TYPE=Debug ..
	cmake --build .
	./bin/cmake_watcher ..
}

build
while inotifywait -e modify src/; do
	build
done