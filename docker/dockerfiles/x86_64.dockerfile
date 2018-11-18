FROM ubuntu:16.04

LABEL author="Sudhanva Sreesha"
LABEL email="sudhanvas@gmail.com"
LABEL github="https://github.com/sudhanvas"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get upgrade --yes

RUN apt-get install -y \
  build-essential \
  ccache \
  cmake \
  git \
  libboost-filesystem-dev \
  libboost-program-options-dev \
  libboost-system-dev \
  libboost-thread-dev \
  pkg-config \
  python

# Install Java JDK and JRE
RUN apt-get install -y default-jdk default-jre
ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64

# Directory to install third-party dependencies
ENV TP_SRC_DIR /usr/local/src
# Directory to download Craebot source code
ENV CB_SRC_DIR /craebot
# Directroy to build Craebot source code
ENV CB_BLD_DIR ${CB_SRC_DIR}/build

# Install ZeroCM
WORKDIR ${TP_SRC_DIR}
RUN git clone https://github.com/ZeroCM/zcm.git
WORKDIR ${TP_SRC_DIR}/zcm
RUN ./waf configure --use-java --use-udpm
RUN ./waf build
RUN ./waf install

# Setup ZCM relate environment variables
ENV CLASSPATH /usr/local/share/java/zcm.jar
ENV CLASSPATH $CLASSPATH:/usr/local/share/java/zcm-tools.jar
ENV CLASSPATH $CLASSPATH:${CB_BLD_DIR}/zcmtypes_craebot.jar

# Cleanup
RUN apt-get autoclean
RUN apt-get clean all

# Init ldconfig
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${CB_BLD_DIR}/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/local/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/lib

# Setup CRAEBOT_ENV for help with building
ENV CRAEBOT_ENV teleop

ENV PATH ${PATH}:${CB_BLD_DIR}/bin
