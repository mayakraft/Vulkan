#!/bin/bash
# source ./.env

# Compile all .vert and .frag files in shaders/
GLSLC=/Library/VulkanSDK/1.4.309.0/macOS/bin/glslc
# GLSLC=$VULKAN_SDK/macOS/bin/glslc
SHADERS_DIR=shaders

for shader in $SHADERS_DIR/*.{vert,frag}; do
  if [ -f "$shader" ]; then
    filename=$(basename -- "$shader")
    ${GLSLC} "$shader" -o "$SHADERS_DIR/${filename}.spv"
  fi
done
