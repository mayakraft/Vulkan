# Compile all .vert and .frag files in shaders/
GLSLC=/Library/VulkanSDK/1.3.268.1/macOS/bin/glslc
SHADERS_DIR=shaders

for shader in $SHADERS_DIR/*.{vert,frag}; do
  if [ -f "$shader" ]; then
    filename=$(basename -- "$shader")
    /Library/VulkanSDK/1.3.268.1/macOS/bin/glslc "$shader" -o "$SHADERS_DIR/${filename}.spv"
  fi
done
