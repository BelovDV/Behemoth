dir_src   = code
project   = suffix_automat

flags     = -D CMAKE_RUNTIME_OUTPUT_DIRECTORY=../bin -D BINARY_NAME=${project}

all:
	@echo root folder

test:
	mkdir -p temp
	mkdir -p temp/build
	mkdir -p temp/bin
	cmake -B temp/build -S ${dir_src}/${project} ${flags}
	cmake --build temp/build

run_test: test
	temp/bin/${project}
 
.PHONY: clean
clean:
	rm -rf temp/**
