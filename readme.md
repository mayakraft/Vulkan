# Vulkan from scratch

## MacOS setup

Prerequisites: Xcode and command line tools.

Install VulkanSDK, which installs MoltenVK. I installed Vulkan into `/Library`

> my folder structure looked like: `/Library/VulkanSDK/1.3.268.1/macOS/include/vulkan/vulkan.h`

### environment variables

I ran these. I suspect the first line is what mattered the most.

```
export DYLD_LIBRARY_PATH=/Library/VulkanSDK/1.3.268.1/macOS/lib
```

```
export VK_ICD_FILENAMES=/Library/VulkanSDK/1.3.268.1/MoltenVK/dylib/macOS/MoltenVK_icd.json
```

```
export VULKAN_SDK=/Library/VulkanSDK/1.3.268.1
```

[source](https://www.reddit.com/r/vulkan/comments/ztxjtw/vulkan_sdk_on_mac_vscode_isnt_working/)

### dependencies

my pkgconfig was messed up. I had to install

```
brew install zeromq
brew install pkgconfig
```

Not needed for all dev environments, but I will use GLFW and GLM, so, install these:

```
brew install glfw
brew install glm
```

due to unrelated issues that I fixed, I ran this, but I suspect that I entirely did not need to:

```
brew install glew glfw3
brew install --cask xquartz
```

### test

Navigate to the first example, /01-setup and modify the path to Vulkan in the .env, then run `make && ./a.out`.

## Windows, Linux setup

not yet tested

## credit

Vulkan starter project from [Brendan Galea](https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=1&pp=iAQB)
