PROJECT_NAME ?= fiber

CMAKE_COMMON_FLAGS ?= -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# Available sanitizers: UBSAN, ASAN, TSAN
CMAKE_DEBUG_FLAGS ?= -DUBSAN=ON -DASAN=ON
CMAKE_RELEASE_FLAGS ?=
NPROCS ?= $(shell nproc)

CLANG_FORMAT ?= clang-format
CLANG_TIDY ?= clang-tidy

DOCKER ?= docker
DOCKER_COMPOSE ?= docker compose

# use Makefile.local for customization
-include Makefile.local

CMAKE_DEBUG_FLAGS += -DCMAKE_BUILD_TYPE=Debug $(CMAKE_COMMON_FLAGS)
CMAKE_RELEASE_FLAGS += -DCMAKE_BUILD_TYPE=Release $(CMAKE_COMMON_FLAGS)

.PHONY: all
all: build-release test-release start-release

.PHONY: cmake-debug
cmake-debug:
	@cmake -B build_debug $(CMAKE_DEBUG_FLAGS)

.PHONY: cmake-release
cmake-release:
	@cmake -B build_release $(CMAKE_RELEASE_FLAGS)

build_debug/CMakeCache.txt: cmake-debug
build_release/CMakeCache.txt: cmake-release

target ?= $(PROJECT_NAME)

.PHONY: build-debug build-release
build-debug build-release: build-%: build_%/CMakeCache.txt
	@cmake --build build_$* -j $(NPROCS) --target $(target)

.PHONY: test-debug test-release
test-debug test-release: test-%:
	@cmake --build build_$* -j $(NPROCS) --target $(PROJECT_NAME)_tests
	@build_$*/tests/$(PROJECT_NAME)_tests

.PHONY: start-debug start-release
start-debug start-release: start-%:
	@executable_path=$$(find build_$* -name $(target) 2>/dev/null); \
	if [ -x $$executable_path ]; then \
		$$executable_path; \
	else \
		echo "Executable not found: $(target)"; \
	fi

.PHONY: clean-debug clean-release
clean-debug clean-release: clean-%:
	@rm -rf build_$*

.PHONY: format
format:
	@find source/$(PROJECT_NAME) tests -name '*pp' -type f | xargs $(CLANG_FORMAT) -i

.PHONY: lint
lint:
	@find source/$(PROJECT_NAME) tests -name '*pp' -type f | xargs $(CLANG_TIDY) -p build_$*

.PHONY: docker-start-debug docker-start-release
docker-start-debug docker-start-release: docker-start-%:
	@$(DOCKER_COMPOSE) -f docker/docker-compose.yml run --rm $(PROJECT_NAME)-container make start-$* target=$(target)

.PHONY: docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release
docker-cmake-debug docker-build-debug docker-test-debug docker-clean-debug docker-cmake-release docker-build-release docker-test-release docker-clean-release: docker-%:
	@$(DOCKER_COMPOSE) -f docker/docker-compose.yml run --rm $(PROJECT_NAME)-container make $* target=$(target)

.PHONY: clean-docker
clean-docker:
	@$(DOCKER_COMPOSE) -f docker/docker-compose.yml down -v --rmi all
