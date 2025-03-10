# First stage: Build the program
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    golang \
    git \
    ninja-build \
    g++ \
    python3 \
    zlib1g-dev \
    libssl-dev  
    # Last two are dependencies for libgit2



# Clone and build libgit2
WORKDIR /libgit2
RUN git clone https://github.com/libgit2/libgit2.git . && \
    mkdir build && cd build && \
    cmake .. && \
    cmake --build . && \
    cmake --install .

# Set working directory
WORKDIR /app

# Clone the repo (or use COPY if building locally)
RUN git clone https://github.com/kevanoovitch/-PA1456-Project-Linter.git LinterRepo
#RUN git clone --branch feature/gitignore-exclusion https://github.com/kevanoovitch/-PA1456-Project-Linter.git LinterRepo

# Build the project
WORKDIR /app/LinterRepo
RUN mkdir -p build && cd build && \
    rm -rf CMakeCache.txt CMakeFiles && \
    cmake .. -G Ninja && \
    cmake --build .



# Second stage: Runtime-only (Final lightweight image)
FROM ubuntu:22.04

# Install minimal runtime dependencies
RUN apt-get update && apt-get install -y \
    zlib1g \
    libssl-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy libgit2 from the build stage
COPY --from=builder /usr/local/lib/libgit2.so* /usr/lib/

# Copy the compiled binary
COPY --from=builder /app/LinterRepo/build/linter /usr/local/bin/linter

# Ensure the binary is executable
RUN chmod +x /usr/local/bin/linter

# Set the default command to run the program
CMD ["/usr/local/bin/linter"]



