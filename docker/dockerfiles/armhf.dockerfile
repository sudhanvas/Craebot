FROM resin/rpi-raspbian

LABEL author="Sudhanva Sreesha"
LABEL email="sudhanvas@gmail.com"
LABEL github="https://github.com/sudhanvas"

COPY docker/qemu-arm-static /usr/bin/qemu-arm-static

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

# Directory to install third-party dependencies
ENV TP_SRC_DIR /usr/local/src
# Directory to download Craebot source code
ENV CB_SRC_DIR /craebot
# Directroy to build Craebot source code
ENV CB_BLD_DIR ${CB_SRC_DIR}/build

# Install Java JDK and JRE
RUN apt-get install -y default-jdk default-jre
ENV JAVA_HOME /usr/lib/jvm/java-7-openjdk-armhf

# Install ZeroCM
WORKDIR ${TP_SRC_DIR}
RUN git clone https://github.com/ZeroCM/zcm.git
WORKDIR ${TP_SRC_DIR}/zcm
RUN ./waf configure --use-java --use-udpm
RUN ./waf build
RUN ./waf install

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

# Init libraries
ENV CLASSPATH /usr/local/share/java/zcm.jar
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${CB_BLD_DIR}/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/local/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/lib
ENV LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/lib

# Setup CRAEBOT_ENV for help with building
ENV CRAEBOT_ENV craebot

# Re-build project for ARM.
COPY . ${CB_SRC_DIR}
WORKDIR ${CB_SRC_DIR}
RUN make clean
RUN rm -rf ${CB_BLD_DIR}
RUN mkdir -p ${CB_BLD_DIR}
WORKDIR ${CB_BLD_DIR}
RUN cmake ..
RUN make -j$(nproc)

ENV PATH ${PATH}:${CB_BLD_DIR}/bin
