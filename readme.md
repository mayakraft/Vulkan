# Vulkan from scratch

I'm learning Vulkan. Here are some resources that I'm using:

- 💻 [Vulkan Tutorial](https://vulkan-tutorial.com/)
- 🎥 [Vulkan Lecture Series, Johannes Unterguggenberger, TU Wien](https://www.youtube.com/playlist?list=PLmIqTlJ6KsE1Jx5HV4sd2jOe3V1KMHHgn)
- 💻 [Vulkan Guide](https://vkguide.dev/)
- 📚 [Vulkan Cookbook by Pawel Lapinski](https://www.packtpub.com/product/vulkan-cookbook/9781786468154)
- 🎥 [Vulkan game engine tutorial by Brendan Galea](https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=1&pp=iAQB)
- 💻 [Yet another blog explaining Vulkan synchronization](https://themaister.net/blog/2019/08/14/yet-another-blog-explaining-vulkan-synchronization/)
- 💻 [Vulkan in 30 minutes](https://renderdoc.org/vulkan-in-30-minutes.html)

## Usage

All numbered folders are self-contained projects. Navigate into any folder and type `make` and then run the program (`make run`, or execute directly `./a.out` or `./bin/app`).

If you are new to Vulkan, you will need to follow some kind of setup:

## MacOS setup

Prerequisites: Xcode command line tools. I'm using makefiles, no need to actually run Xcode or install CMake. But, I think Xcode needs to be installed anyway.

Install [VulkanSDK](https://vulkan.lunarg.com/sdk/home#mac), which installs MoltenVK. I installed Vulkan into `/Library`

> my folder structure looked like: `/Library/VulkanSDK/1.3.268.1/macOS/include/vulkan/vulkan.h`

### system environment variables

The app will not run unless your system knows the location of the Vulkan library. You can do it two ways, either a quick fix by running these in the terminal window (however, you will need to run them every time you open a new terminal, so it's really not ideal):

```sh
export VULKAN_SDK=/Library/VulkanSDK/1.3.268.1
export DYLD_LIBRARY_PATH=/Library/VulkanSDK/1.3.268.1/macOS/lib
```

or the better solution, modify your `~/.zshrc` or `~/.bashrc` file (re-open a new terminal window):

```sh
# Vulkan SDK setup
export VULKAN_SDK=/Library/VulkanSDK/1.3.268.1
export DYLD_LIBRARY_PATH=$VULKAN_SDK/macOS/lib:$DYLD_LIBRARY_PATH
```

As a footnote, I'm not sure how important this is to set:

```sh
export VK_ICD_FILENAMES=/Library/VulkanSDK/1.3.268.1/MoltenVK/dylib/macOS/MoltenVK_icd.json
```

[source](https://www.reddit.com/r/vulkan/comments/ztxjtw/vulkan_sdk_on_mac_vscode_isnt_working/)

### dependencies

my pkgconfig was messed up. I had to install

```sh
brew install zeromq
brew install pkgconfig
```

Not needed for all dev environments, but I will use GLFW and GLM, so, install these:

```sh
brew install glfw
brew install glm
```

due to unrelated issues that I fixed, I ran this, but I suspect that I entirely did not need to:

```sh
brew install glew glfw3
brew install --cask xquartz
```

### project setup

Every project has an `.env` file. You have to modify it to match the location of the Vulkan SDK on your computer.

## Windows, Linux setup

not yet
