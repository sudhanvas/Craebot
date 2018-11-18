ARG repository=ubuntu:16.04
FROM $repository

LABEL author="Sudhanva Sreesha"
LABEL email="sudhanvas@gmail.com"
LABEL github="https://github.com/sudhanvas"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get upgrade --yes

# Install tools
RUN apt-get update
RUN apt-get install -y \
    --fix-missing \
    apt-transport-https \
    apt-utils \
    ca-certificates \
    curl \
    git \
    net-tools \
    python-software-properties \
    software-properties-common \
    sudo \
    vim

# Install build tools
RUN apt-get install -y \
    build-essential \
    ccache \
    clang \
    clang-format \
    clang-tidy \
    cmake \
    gdb \
    libboost-all-dev \
    libxslt-dev \
    valgrind

# Install Python
RUN apt-get install -y \
    python \
    python-dev \
    python-pip \
    python3 \
    python3-dev \
    python3-pip \
    virtualenv
RUN pip install --upgrade pip

# Install Java JDK and JRE
RUN apt-get install -y default-jdk default-jre
ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64

# Install third-party libraries
ENV TP_SRC_DIR /usr/local/src

# Install ZeroCM
WORKDIR ${TP_SRC_DIR}
RUN apt-get install -y \
    clang \
    cxxtest \
    libelf-dev \
    libelf1 \
    libzmq3-dev \
    mlocate \
    nodejs \
    nodejs-legacy \
    npm \
    pkg-config
RUN pip install Cython
RUN updatedb
RUN git clone https://github.com/ZeroCM/zcm.git
WORKDIR ${TP_SRC_DIR}/zcm
RUN ./waf configure \
    --use-java \
    --use-python \
    --use-zmq \
    --use-elf \
    --use-third-party \
    --use-inproc \
    --use-udpm \
    --use-serial \
    --use-ipc \
    --use-dev
RUN ./waf build
RUN ./waf install

# Setup ZCM related environment variables
ENV ZCM_SPY_LITE_PATH /usr/local/lib
ENV CLASSPATH /usr/local/share/java/zcm.jar
ENV CLASSPATH $CLASSPATH:/usr/local/share/java/zcm-tools.jar
ENV CLASSPATH $CLASSPATH:/craebot/build/zcmtypes_craebot.jar

# Install libcreate
WORKDIR ${TP_SRC_DIR}
RUN git clone https://github.com/AutonomyLab/libcreate.git
WORKDIR ${TP_SRC_DIR}/libcreate
RUN mkdir build
WORKDIR ${TP_SRC_DIR}/libcreate/build
RUN cmake ..
RUN make -j$(nproc)
RUN make -j$(nproc) install

# Cleanup
RUN apt-get autoclean
RUN apt-get clean all

# Init ldconfig
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/craebot/build/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/local/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/lib

# Enables building the source code for dev on x86_64
ENV CRAEBOT_ENV dev
