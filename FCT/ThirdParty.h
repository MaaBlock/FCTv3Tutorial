//#define FCT_USE_GLAD
#define FCT_USE_GLFW
#define FCT_USE_PHYSX
#define FCT_USE_FREETYPE
#define FCT_USE_HARFBUZZ
#define FCT_USE_BULLET
#define FCT_USE_VULKAN
#ifdef _WIN32
	#define NOMINMAX
	#include <Windows.h>
#endif

#ifdef __ANDROID__
    #define FCT_USE_ANDROID
	#undef FCT_USE_GLAD
	#undef FCT_USE_GLFW
	#undef FCT_USE_PHYSX
    #define FCT_ANDROID
    #define FCT_USE_JNICPP
#endif

#ifdef FCT_USE_GLAD
	#include <glad/glad.h>
#endif

#ifdef FCT_USE_GLFW
	#include <glfw/glfw3.h>
#endif

//#include <boost/lockfree/queue.hpp>
#ifdef FCT_USE_PHYSX
	#include <PxPhysicsAPI.h>
#endif

#ifdef FCT_USE_FREETYPE
//#include <ft2build.h>
//#include FT_FREETYPE_H
//#include FT_COLOR_H
#endif

#ifdef FCT_USE_HARFBUZZ
//#include <harfbuzz/hb.h>
//#include <harfbuzz/hb-ft.h>
#endif

#ifdef FCT_USE_BULLET
//#include <btBulletDynamicsCommon.h>
#endif

#include <boost/lockfree/queue.hpp>
#include <thread>
#include <map>
#include <functional>
#include <memory>

#ifdef FCT_ANDROID
#include <android/configuration.h>
#include <android/looper.h>
#include <android/asset_manager.h>
#include <android/input.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/rect.h>
#include <android/looper.h>
#include "./ThirdParty/jni_bind_release.h"
#include <jni.h>
#include <sys/types.h>
#endif

#ifdef FCT_USE_VULKAN
#include <vulkan/vulkan.h>
#ifdef FCT_ANDROID
extern int main();
#include <vulkan/vulkan_android.h>
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#define VK_NO_PROTOTYPES
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define FCT_DEBUG_MODE
#define FCT_DEBUG
#endif

#include <future>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>