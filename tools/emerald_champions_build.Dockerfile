FROM debian:bookworm-slim

RUN apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      build-essential \
      binutils-arm-none-eabi \
      gcc-arm-none-eabi \
      git \
      libpng-dev \
      libnewlib-arm-none-eabi \
      python3 \
      zlib1g-dev \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /work
CMD ["make", "-j4"]
