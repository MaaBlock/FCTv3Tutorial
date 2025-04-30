#ifndef FCT_THRIDPARTY_HEADER
#define FCT_THIRDPARTY_HEADER
//#define FCT_USE_GLAD
#define FCT_USE_GLFW
#define FCT_USE_PHYSX
#define FCT_USE_FREETYPE
#define FCT_USE_HARFBUZZ
#define FCT_USE_BULLET
#define FCT_USE_VULKAN
#define FCT_USE_SPIRV
#define FCT_USE_SHADERC
#define FCT_USE_FREEIMAGE
#define FCT_USE_ASSIMP
#define FCT_USE_ENTT
#ifdef _WIN32
	#define FCT_WIN32
	#define NOMINMAX
	#include <Windows.h>
#endif

#ifdef __ANDROID__
    #define FCT_USE_ANDROID
	#undef FCT_USE_GLAD
	#undef FCT_USE_GLFW
	#undef FCT_USE_PHYSX
	#undef FCT_USE_FREEIMAGE
    #define FCT_ANDROID
    #define FCT_USE_JNICPP
#endif

#ifdef FCT_USE_GLAD
	#include <glad/glad.h>
#endif

#ifdef FCT_USE_GLFW
#ifdef FCT_USE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>

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
#ifdef FCT_WIN32
#include <vulkan/vulkan_win32.h>
#endif
#ifdef FCT_ANDROID
extern int main();
#include <vulkan/vulkan_android.h>
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#define VK_NO_PROTOTYPES
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#endif

#ifdef FCT_USE_SPIRV
#include <spirv_reflect.hpp>
#include <spirv_hlsl.hpp>
#endif

#ifdef FCT_USE_SHADERC
#include <shaderc/shaderc.hpp>
#endif

#ifdef FCT_USE_FREEIMAGE
#include <FreeImage.h>
#endif
#ifdef FCT_USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

#ifdef FCT_USE_ENTT
#include "./ThirdParty/entt.hpp"
namespace FCT {
	namespace ECS = entt;
}
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
#include <fstream>
#include <filesystem>
#include <set>
#include <queue>
#endif // FCT_THIRDPARTY_HEADER