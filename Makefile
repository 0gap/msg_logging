.DEFAULT_GOAL:= build-debug

clean:
	rm -rf cmake-build-debug


build-debug: clean
	mkdir -p cmake-build-debug/generated
	cd cmake-build-debug && conan install .. && cd generated && flatc --cpp --scoped-enums --grpc ../../log_msg.fbs
	cd cmake-build-debug && cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd cmake-build-debug && cmake --build . -- -j 4
