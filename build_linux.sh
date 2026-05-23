# Image generated with https://github.com/godotengine/build-containers.
# using ./build.sh 4.7 f43

podman run --rm \
  -v $(pwd):/godot-libbf:Z \
  -w /godot-libbf \
  -e CXX=/root/x86_64-godot-linux-gnu_sdk-buildroot/bin/g++ \
  -e CC=/root/x86_64-godot-linux-gnu_sdk-buildroot/bin/gcc \
  -e AR=/root/x86_64-godot-linux-gnu_sdk-buildroot/bin/gcc-ar \
  -e PATH=/root/x86_64-godot-linux-gnu_sdk-buildroot/bin:$PATH \
  localhost/godot-linux:4.7-f43 \
  scons platform=linux target=template_debug