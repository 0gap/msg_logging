.DEFAULT_GOAL:= build-release

flat:
	mkdir -p cmake-build-debug/generated
	cd cmake-build-debug/generated && flatc --cpp --scoped-enums --grpc ../../log_msg.fbs
	mkdir -p cmake-build-release/generated
	cd cmake-build-release/generated && flatc --cpp --scoped-enums --grpc ../../log_msg.fbs

build-debug:
	rm -rf cmake-build-debug
	mkdir -p cmake-build-debug
	cd cmake-build-debug && conan install -pr debug ..
	cd cmake-build-debug && cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd cmake-build-debug/bin && conan info ../.. --graph=dependency_graph.html

build-release:
	rm -rf cmake-build-release
	mkdir -p cmake-build-release
	cd cmake-build-release && conan install -pr release ..
	cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release ..
	cd cmake-build-release && cmake --build .
	cd cmake-build-release/bin && conan info ../.. --graph=dependency_graph.html

lb-down:
	docker-compose -f ./load_balance/nginx_config/nginx-docker-compose.yml down
	docker-compose -f ./load_balance/haconfig/ha-docker-compose.yml down

benchmarks: lb-down build-release
	docker-compose -f ./load_balance/nginx_config/nginx-docker-compose.yml up -d
	docker-compose -f ./load_balance/haconfig/ha-docker-compose.yml up -d
	/bin/bash -c "cmake-build-release/bin/load_balance_benchmarks"
