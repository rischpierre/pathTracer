FROM gcc:9.5

RUN apt-get -y update

RUN apt-get -y install wget git
RUN apt-get -y install gdb

# Cmake custom install because the ppa repo is not up to date for USD
WORKDIR /tmp

RUN apt-get install -y libssl-dev   # required for cmake
ARG CMAKE_VERSION_FULL=3.22.5
ARG CMAKE_VERSION_MINOR=3.22
RUN wget -q https://cmake.org/files/v${CMAKE_VERSION_MINOR}/cmake-${CMAKE_VERSION_FULL}.tar.gz
RUN tar xzf cmake-${CMAKE_VERSION_FULL}.tar.gz
WORKDIR /tmp/cmake-${CMAKE_VERSION_FULL}
RUN ./configure
RUN make
RUN make install
RUN make clean
RUN rm -rf /tmp/cmake-${CMAKE_VERSION_FULL}
RUN rm /tmp/cmake-${CMAKE_VERSION_FULL}.tar.gz

# Install USD
WORKDIR /tmp
ARG USD_VERSION=19.11
ARG USD_INSTALL_ROOT=/opt/USD

RUN git clone -b v${USD_VERSION} https://github.com/PixarAnimationStudios/USD
RUN python /tmp/USD/build_scripts/build_usd.py  \
    --no-embree \
    --no-maya \
    --no-houdini  \
    --no-imaging  \
    --no-python \
    --no-ptex \
    --no-usdview \
    --no-prman \
    --no-docs \
    --no-tests \
    --no-draco \
    ${USD_INSTALL_ROOT}

RUN rm -rf /tmp/USD
