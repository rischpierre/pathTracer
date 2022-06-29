# todo try to install using gcc image in order to be linux distribution agnostic
FROM ubuntu:18.04

RUN apt-get -y update

RUN apt-get -y install build-essential
RUN apt-get -y install wget git
RUN apt-get -y install python3 python3-pip

WORKDIR /tmp

# install precompiled cmake to have a newer version (needed for USD compilation)
ARG cmakeVersion=3.23.2
RUN wget -nv https://github.com/Kitware/CMake/releases/download/v$cmakeVersion/cmake-$cmakeVersion-linux-x86_64.tar.gz
RUN tar -xzvf cmake-$cmakeVersion-linux-x86_64.tar.gz

# todo copy cmake to /usr/bin
ENV PATH="/tmp/cmake-$cmakeVersion-linux-x86_64/bin:${PATH}"
RUN echo $PATH

WORKDIR /home

# Install USD
RUN pip3 install PySide2 PyOpenGL
RUN git clone -b v21.11 https://github.com/PixarAnimationStudios/USD
RUN python3 USD/build_scripts/build_usd.py --no-python /usr/local/USD
