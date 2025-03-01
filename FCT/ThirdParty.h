#define FCT_USE_GLAD
#define FCT_USE_GLFW
#define FCT_USE_PHYSX
#define FCT_USE_FREETYPE
#define FCT_USE_HARFBUZZ
#define FCT_USE_BULLET
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
#endif

#ifdef FCT_USE_GLAD
	#include <glad/glad.h>
#endif

#ifdef FCT_USE_GLFW
	#include <glfw/glfw3.h>
#endif

#include <boost/lockfree/queue.hpp>
#ifdef FCT_USE_PHYSX
	#include <PxPhysicsAPI.h>
#endif

#ifdef FCT_USE_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_COLOR_H
#endif

#ifdef FCT_USE_HARFBUZZ
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#endif

#ifdef FCT_USE_BULLET
#include <btBulletDynamicsCommon.h>
#endif

#include <boost/lockfree/queue.hpp>
#include <thread>
#include <map>
#include <functional>
#include <memory>

#ifdef FCT_ANDROID
#include <android/asset_manager.h>
#include <android/input.h>
#include <android/native_window.h>
#include <android/rect.h>
#include <android/looper.h>
#include <jni.h>
#include <sys/types.h>
#endif

#ifdef _DEBUG
#define FCT_DEBUG_MODE
#endif