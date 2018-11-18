# Docker Support

Docker containers have emerged as a solution to running code or services in a way that is isolated from the host operating system. This allows code to be compiled and run on systems running unsupported operating systems or with incompatible configurations to the software dependencies. Docker is available for [all major operating systems][docker_download].

Craebot provides Docker support for two architectures. There are Dockerfiles that use different base images based on the system architectures. The Docker containers are dedicated for development without needing to configure a supported operating system.

When building a Docker container on a "x86_64" architecture, the Dockerfile builds an isolated Ubuntu 16.04 environment that can run Craebot; this container should typically be the "base station" (i.e. tele-operator). The packages installed in this container makeup a superset among the two.

On the other hand, when building a container on the "armv6l" or the "armv7l" architectures, the Dockerfile builds an isolated Raspbian environment that can run Craebot; this container is dedicated for the Raspberry Pi housed on the Create robot. THis container installs only a subset of the packages from the former mostly to speed up building the container directly on the Pi. However, feel free to use the "x86_64" container on the robot, as well, if the CPU architecture permits.

Note: The Dockerfile is provided as an experimental feature for both the Raspberry Pi and regular machines.

## Instructions

### Docker Installation

In order to get docker installed, please follow guides specific to your operating system. The typical steps are:

1. Install docker from your distribution’s package manager or official installer from [Docker][docker_store].
2. Enable Docker to run as a daemon/service.
3. Add appropriate users to the docker group to give permissions to interact with the Docker service.
4. Log out and back in to update user groups.

### Building Containers

The standard workflow for getting these containers running is (from the main Craebot directory):

```bash
bash docker/scripts/build.sh
bash docker/scripts/run.sh -c CONTAINER_NAME -i IMAGE_NAME
bash docker/scripts/login.sh -c CONTAINER_NAME
```

If successful, `docker images` should show an image named `craebot:{arch}` and `docker ps` will show any containers running on your system.

Happy Dockering!

[docker_download]: https://www.docker.com/community-edition#/download
[docker_store]: https://store.docker.com
