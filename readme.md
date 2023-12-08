# Vulkan from scratch

I'm learning Vulkan. Here are some resources that I'm using:

- 📖 [Vulkan Cookbook by Pawel Lapinski](https://www.packtpub.com/product/vulkan-cookbook/9781786468154)
- 📺 [Vulkan game engine tutorial by Brendan Galea](https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=1&pp=iAQB)
- 💻 [Vulkan Tutorial](https://vulkan-tutorial.com/)
- 💻 [Vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)
- 💻 [API without secrets](https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-preface.html)
- 💻 [Yet another blog explaining Vulkan synchronization](https://themaister.net/blog/2019/08/14/yet-another-blog-explaining-vulkan-synchronization/)
- 💻 [Vulkan Guide](https://vkguide.dev/)

## Usage

All numbered folders are self-contained projects. Navigate into any folder and type `make` and then run the program (usually `./a.out`).

But, it probably will not work if you don't follow this setup.

## MacOS setup

Prerequisites: Xcode command line tools. I'm using makefiles, no need to actually run Xcode or install CMake. But, I think Xcode needs to be installed anyway.

Install [VulkanSDK](https://vulkan.lunarg.com/sdk/home#mac), which installs MoltenVK. I installed Vulkan into `/Library`

> my folder structure looked like: `/Library/VulkanSDK/1.3.268.1/macOS/include/vulkan/vulkan.h`

### system environment variables

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

### project setup

Every project has an `.env` file. You have to modify it to match the location of the Vulkan SDK on your computer.

## Windows, Linux setup

not yet
