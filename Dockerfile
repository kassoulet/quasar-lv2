# Use the latest Ubuntu image as the base
FROM ubuntu:latest

# Update the package lists
RUN apt-get update -y

# Install necessary build tools and libraries
RUN apt-get install -y build-essential git pkg-config lv2-dev ladspa-sdk liblilv-dev libboost-dev meson lv2-dev sordi valgrind lilv-utils libglib2.0-dev liblilv-dev libsndfile1-dev

# Build plugin-torture
RUN git clone https://github.com/cth103/plugin-torture /plugin-torture
WORKDIR /plugin-torture
RUN make -j$(nproc)

# Build lv2bm
RUN git clone https://github.com/mod-audio/lv2bm.git /lv2bm
WORKDIR /lv2bm
RUN pwd
RUN ls -la
RUN make -j$(nproc)

# Build plugin
WORKDIR /quasar-lv2

# Copy source code to the Docker image
COPY manifest.ttl.in  meson.build  meson_options.txt  quasar.c  quasar.ttl ./

# Validate ttl files
RUN lv2_validate manifest.ttl.in quasar.ttl

# Compile plugin
RUN meson setup build -Dlv2dir=$HOME/.lv2/
RUN meson compile -C build
RUN meson install -C build

# Run and Check benchmark
# RUN valgrind --leak-check=full --show-reachable=no /lv2bm/lv2bm --full-test https://github.com/kassoulet/quasar-lv2

# Run the plugin-torture
RUN /plugin-torture/plugin-torture -d -a --lv2 --plugin $HOME/.lv2/quasar.lv2/manifest.ttl
RUN /plugin-torture/plugin-torture -e --lv2 --plugin $HOME/.lv2/quasar.lv2/manifest.ttl
# CMD gdb --args /plugin-torture/plugin-torture -e -d -a --lv2 --plugin $HOME/.lv2/quasar.lv2/manifest.ttl

# Run benchmark
CMD /lv2bm/lv2bm --full-test https://github.com/kassoulet/quasar-lv2
