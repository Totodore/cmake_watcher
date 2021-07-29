#!/bin/bash

build() {
	echo "Building..."
	cd build/
	cmake -DCMAKE_BUILD_TYPE=Debug ..
	cmake --build .
}

build