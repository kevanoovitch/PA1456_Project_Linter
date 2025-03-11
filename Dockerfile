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
    unzip \
    wget \
    zlib1g-dev \
    libssl-dev  
    # Last two are dependencies for libgit2

#  Install gitleaks 8.24.0
RUN wget -q https://github.com/gitleaks/gitleaks/releases/download/v8.24.0/gitleaks_8.24.0_linux_x32.tar.gz -O /tmp/gitleaks.tar.gz && \
tar -xzf /tmp/gitleaks.tar.gz -C /tmp && \
mv /tmp/gitleaks /usr/local/bin/gitleaks && \
chmod +x /usr/local/bin/gitleaks


# Clone and build libgit2
WORKDIR /libgit2
RUN git clone https://github.com/libgit2/libgit2.git . && \
    mkdir build && cd build && \
    cmake .. && \
    cmake --build . && \
    cmake --install .

# Set working directory
WORKDIR /app


COPY . /app/LinterRepo


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
    libssl-dev \
    ca-certificates && \
    apt-get clean && rm -rf /var/lib/apt/lists/*





# Copy libgit2 from the build stage
COPY --from=builder /usr/local/lib/libgit2.so* /usr/lib/
# Copy gitleaks from the build stage
COPY --from=builder /usr/local/bin/gitleaks /usr/local/bin/gitleaks

# Copy the compiled binary
COPY --from=builder /app/LinterRepo/build/linter /usr/linterApp/build/linter
COPY --from=builder /app/LinterRepo/config.json /usr/linterApp/config.json

# Copy the test related files
COPY --from=builder /app/LinterRepo/build/linterTests /usr/linterApp/build/linterTests
RUN  mkdir -p /usr/linterApp/Tests/testDir 
COPY --from=builder /app/LinterRepo/Tests/dummyRepos /usr/linterApp/Tests/dummyRepos  
COPY --from=builder /app/LinterRepo/Tests/dummyRepos /usr/linterApp/Tests/dummyRepos  
COPY --from=builder /app/LinterRepo/Tests/dummyRepos /usr/linterApp/Tests/dummyRepos  
COPY --from=builder /app/LinterRepo/Tests/testConfigDefaults.json /usr/linterApp/Tests/testConfigDefaults.json
COPY --from=builder /app/LinterRepo/Tests/testConfigNoScan.json /usr/linterApp/Tests/testConfigNoScan.json
# Set the default command to run the program
WORKDIR /usr/linterApp/build
CMD ["/usr/linterApp/build/linter"]



