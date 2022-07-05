FROM ubuntu:18.04

RUN apt-get -y update

RUN apt-get -y install build-essential
RUN apt-get -y install wget git
RUN apt-get -y install python3 python3-pip
RUN apt-get -y install gdb

# Cmake custom install because the ppa repo is not up to date for USD
WORKDIR /tmp
RUN apt-get install -y libssl-dev   # required for cmake
ARG CMAKE_VERSION_FULL=3.23.2
ARG CMAKE_VERSION_MINOR=3.23
RUN wget https://cmake.org/files/v${CMAKE_VERSION_MINOR}/cmake-${CMAKE_VERSION_FULL}.tar.gz
RUN tar xzf cmake-${CMAKE_VERSION_FULL}.tar.gz
WORKDIR /tmp/cmake-${CMAKE_VERSION_FULL}
RUN ./configure
RUN make
RUN make install
WORKDIR /home

# Install USD
# todo need to fix bug with openGL
# RUN pip3 install PySide2 PyOpenGL
# RUN git clone -b v21.11 https://github.com/PixarAnimationStudios/USD
# RUN python3 USD/build_scripts/build_usd.py --no-python /usr/local/USD
