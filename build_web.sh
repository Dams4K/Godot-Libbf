# Image generated with https://github.com/godotengine/build-containers.
# using ./build.sh 4.7 f43

podman run --rm \
  -v $(pwd):/godot-libbf:Z \
  -w /godot-libbf \
  -e PATH=/root/emsdk/upstream/emscripten:/root/emsdk/upstream/bin:/usr/local/bin:/usr/bin \
  localhost/godot-web:4.7-f43 \
  bash -c 'scons platform=web target=template_debug && scons platform=web target=template_release'