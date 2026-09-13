FROM ubuntu:24.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    binutils-arm-none-eabi build-essential ca-certificates gcc-arm-none-eabi git \
    libmgba-dev libnewlib-arm-none-eabi libpng-dev pkg-config python3 python3-pil \
    fonts-dejavu-core && rm -rf /var/lib/apt/lists/*
WORKDIR /game
CMD ["bash"]
