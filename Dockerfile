FROM gcc:latest AS builder

WORKDIR /app

COPY CMakeLists.txt .
COPY src/ src/
COPY include/ include/
COPY examples/ examples/
COPY tests/ tests/

RUN apt-get update && apt-get install -y cmake && rm -rf /var/lib/apt/lists/*

RUN mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_EXE_LINKER_FLAGS="-static" && \
    cmake --build . --config Release

FROM debian:bullseye-slim

WORKDIR /app
COPY --from=builder /app/build/bin/Release/ /app/bin/
COPY docker-entrypoint.sh /app/

RUN chmod +x /app/docker-entrypoint.sh
ENTRYPOINT ["/app/docker-entrypoint.sh"]
CMD ["example"]