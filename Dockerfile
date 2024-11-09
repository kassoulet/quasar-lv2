# Use the latest Ubuntu image as the base
FROM ubuntu:latest

# Update the package lists
RUN apt-get update -y

# Install necessary build tools and libraries
RUN apt-get install -y build-essential git libtool automake pkg-config lv2-dev ladspa-sdk liblilv-dev libboost-all-dev meson ninja-build

# Clone the plugin-torture repository
RUN git clone https://github.com/cth103/plugin-torture

# Build
WORKDIR /plugin-torture
RUN make -j$(nproc)

WORKDIR /quasar-lv2

# Copy source code to the Docker image
COPY manifest.ttl.in  meson.build  meson_options.txt  quasar.c  quasar.ttl ./

# Compile plugin
RUN meson setup build -Dlv2dir=$HOME/.lv2/
RUN meson compile -C build
RUN meson install -C build

# Run the plugin-torture
CMD /plugin-torture/plugin-torture -d -a --lv2 --plugin $HOME/.lv2/quasar.lv2/manifest.ttl