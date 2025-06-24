FROM debian:bullseye

RUN apt-get update && apt-get install -y \
  build-essential \
  libssl-dev \
  libcurl4-openssl-dev \
  zlib1g-dev \
  libwebsocketpp-dev \
  libboost-system-dev \
  libboost-thread-dev \
  pkg-config \
  git \
  curl \
  && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/brainboxdotcc/DPP.git && \
    cd DPP && \
    cmake . && \
    make install

WORKDIR /app
COPY . .

RUN make

CMD ["./aurebesh-droid"]