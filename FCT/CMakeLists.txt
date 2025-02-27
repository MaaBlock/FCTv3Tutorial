file(GLOB UI_MODULE_SOURCES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "./UI/[^_]*[^_].cpp"
)
file(GLOB UI_MODULE_HEADERS
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "./UI/[^_]*[^_].h"
)

file(GLOB UI_ANDROID_MODULE_SOURCES
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "./UI/Android_*.cpp"
)
file(GLOB UI_ANDROID_MODULE_HEADERS
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "./UI/Android_*.h"
)

file(GLOB RUNTIME_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/[^_]*[^_].cpp"
)

message("RUNTIME_MODULE_SOURCES: ${RUNTIME_MODULE_SOURCES}")

file(GLOB RUNTIME_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/[^_]*[^_].h"
)

file(GLOB RUNTIME_ANDROID_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/Android_*.cpp"
)
file(GLOB RUNTIME_ANDROID_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/Android_*.h"
)

find_package(Boost REQUIRED COMPONENTS system thread locale)
find_package(harfbuzz CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(Bullet CONFIG REQUIRED)


add_library(FCT STATIC
    ${UI_MODULE_SOURCES}
    ${UI_ANDROID_MODULE_SOURCES}
    ${RUNTIME_MODULE_SOURCES}
    headers.cpp
)

target_link_libraries(FCT PUBLIC
    harfbuzz::harfbuzz
    harfbuzz::harfbuzz-subset
    Freetype::Freetype
    ${Boost_LIBRARIES}
    ${BULLET_LIBRARIES}
)
