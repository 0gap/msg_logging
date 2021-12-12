.DEFAULT_GOAL:= build-release

build-debug:
	rm -rf cmake-build-debug
	mkdir -p cmake-build-debug/generated
	cd cmake-build-debug && conan install .. && cd generated && flatc --cpp --scoped-enums --grpc ../../log_msg.fbs
	cd cmake-build-debug && cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd cmake-build-debug/bin && conan info ../.. --graph=dependency_graph.html

build-release:
	rm -rf cmake-build-release
	mkdir -p cmake-build-release/generated
	cd cmake-build-release && conan install .. && cd generated && flatc --cpp --scoped-enums --grpc ../../log_msg.fbs
	cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd cmake-build-release && cmake --build . -- -j 4
	cd cmake-build-release/bin && conan info ../.. --graph=dependency_graph.html

benchmarks: build-release
	docker-compose -f ./load_balance/nginx_config/nginx-docker-compose.yml down
	docker-compose -f ./load_balance/haconfig/ha-docker-compose.yml down
	docker-compose -f ./load_balance/nginx_config/nginx-docker-compose.yml up -d
	docker-compose -f ./load_balance/haconfig/ha-docker-compose.yml up -d
	/bin/bash -c "cmake-build-release/bin/load_balance_benchmarks"
