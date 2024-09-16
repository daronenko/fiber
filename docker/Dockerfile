FROM ubuntu:latest

WORKDIR /fiber

RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates \
        sudo \
        git \
        make \
        cmake \
        clang-17 \
        libc++-17-dev \
  	    libc++abi-17-dev \
	      libclang-rt-17-dev \
        gdb \
        ccache && \
    rm -rf /var/lib/apt/lists/*
