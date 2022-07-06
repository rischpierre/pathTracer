FROM gcc:9.5

RUN apt-get -y update

RUN apt-get -y install wget git
RUN apt-get -y install gdb

# Cmake custom install because the ppa repo is not up to date for USD
WORKDIR /tmp

RUN apt-get install -y libssl-dev   # required for cmake
ARG CMAKE_VERSION_FULL=3.23.2
ARG CMAKE_VERSION_MINOR=3.23
RUN wget -q https://cmake.org/files/v${CMAKE_VERSION_MINOR}/cmake-${CMAKE_VERSION_FULL}.tar.gz
RUN tar xzf cmake-${CMAKE_VERSION_FULL}.tar.gz
WORKDIR /tmp/cmake-${CMAKE_VERSION_FULL}
RUN ./configure
RUN make
RUN make install
RUN make clean


# install boost
WORKDIR /opt
RUN cd /home && wget -q http://downloads.sourceforge.net/project/boost/boost/1.60.0/boost_1_60_0.tar.gz \
  && tar xfz boost_1_60_0.tar.gz \
  && rm boost_1_60_0.tar.gz \
  && cd boost_1_60_0 \
  && ./bootstrap.sh --prefix=/usr/local --with-libraries=program_options \
  && ./b2 install \
  && cd /home \
  && rm -rf boost_1_60_0

# TODO Download and install TBB
ENV TBB_RELEASE 2018_U2
ENV TBB_VERSION 2018_20171205
ENV TBB_DOWNLOAD_URL https://github.com/01org/tbb/releases/download/${TBB_RELEASE}/tbb${TBB_VERSION}oss_lin.tgz
ENV TBB_INSTALL_DIR /opt
RUN wget ${TBB_DOWNLOAD_URL} && \
	tar -C ${TBB_INSTALL_DIR} -xf tbb${TBB_VERSION}oss_lin.tgz && \
	rm tbb${TBB_VERSION}oss_lin.tgz && \
	sed -i "s%SUBSTITUTE_INSTALL_DIR_HERE%${TBB_INSTALL_DIR}/tbb${TBB_VERSION}oss%" ${TBB_INSTALL_DIR}/tbb${TBB_VERSION}oss/bin/tbbvars.* && \
	echo "source ${TBB_INSTALL_DIR}/tbb${TBB_VERSION}oss/bin/tbbvars.sh intel64" >> ~/.bashrc \

# Install USD
ARG USD_ROOT=/opt/USD
ARG USD_VERSION=21.11
ARG USD_BUILD_DIR=$USD_ROOT/build

RUN git clone -b v21.11 https://github.com/PixarAnimationStudios/USD $USD_ROOT

RUN mkdir $USD_BUILD_DIR
WORKDIR $USD_BUILD_DIR
RUN echo $BOOST_ROOT


RUN cmake \
      -DCMAKE_INSTALL_PREFIX=$USD_BUILD_DIR/usd/${USD_VERSION} \
      -DCMAKE_PREFIX_PATH=$USD_BUILD_DIR \
      -DPXR_BUILD_TESTS=OFF \
      -DTBB_ROOT_DIR=$USD_ROOT\
      -DBOOST_ROOT_DIR=$BOOST_ROOT \
      -DPXR_ENABLE_GL_SUPPORT=FALSE \
      -DPXR_ENABLE_PTEX_SUPPORT=FALSE \
      -DPXR_ENABLE_PYTHON_SUPPORT=FALSE \
      -DPXR_MALLOC_LIBRARY:path=$USD_ROOT/lib/libjemalloc.so \
      -DPXR_BUILD_USD_IMAGING=FALSE \
      ..

RUN cd $TMP_DIR/USD-${USD_VERSION}/build && \
    make -j 4

RUN cd $TMP_DIR/USD-${USD_VERSION}/build && \
    make install