#!/bin/bash
set -e  # Stop script on error
echo "🔧 Setting up the build environment..."

# Get the directory of the script
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

# Detect macOS SDK path
SDK_PATH=$(xcrun --sdk macosx --show-sdk-path)

# Set compiler flags
CXX_FLAGS="-isysroot $SDK_PATH -stdlib=libc++"
BUILD_TYPE="Release"

# Optional: Use Homebrew Clang if installed
if command -v brew &> /dev/null && brew list llvm &> /dev/null; then
    echo "🍺 Using Homebrew Clang..."
    export CC=/opt/homebrew/opt/llvm/bin/clang
    export CXX=/opt/homebrew/opt/llvm/bin/clang++
fi

# Custom env setup
export GST_PLUGIN_PATH="/opt/homebrew/Cellar/gstreamer/1.24.12_1/lib/gstreamer-1.0"
export PKG_CONFIG_PATH="/opt/homebrew/Cellar/gtk4/4.16.12/lib/pkgconfig:$PKG_CONFIG_PATH"

# Cleanup previous build
echo "🧹 Cleaning up old build files..."
rm -rf build
mkdir -p build
cd build

# Build Live555 (if not already built)
LIVE555_DIR="${SCRIPT_DIR}/external/live555"
if [ ! -f "$LIVE555_DIR/liveMedia/libliveMedia.a" ]; then
    echo "🛠️ Building Live555..."
    cd $LIVE555_DIR
    ./genMakefiles macosx-no-openssl # in case of macOS (in case of linux: `linux`)
    make -j$(nproc)
    cd -  # Go back to the build directory
else
    echo "✅ Live555 already built, skipping..."
fi

# Run CMake with proper flags
echo "⚙️ Running CMake..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
      -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
      -DFRUIT_ENABLE_TESTS=OFF \
      -DUSE_LIBCURL=ON \
      -DNO_OPENSSL=ON ..

# Build the project
echo "🚀 Building the project..."
make -j$(nproc)

echo "✅ Build completed successfully!"
