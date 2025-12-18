# 设置cmake最低版本
cmake_minimum_required(VERSION 3.21)

project(JQChat LANGUAGES CXX)

# 导出所有符号
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
# 设置camke搜索头文件时包含当前源文件所在目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 多配置生成器（Visual Studio, Xcode）
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})        #CMAKE_CONFIGURATION_TYPES 可能的值：Debug, Release, MinSizeRel, RelWithDebInfo
    string(TOUPPER ${CONFIG} CONFIG_UPPER)          # 将字符串转换为大写
    message(STATUS "Config: ${CONFIG}")
    message(STATUS "Config_upper: ${CONFIG_UPPER}")
    set_target_properties(${ADDLIB} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${ADDLIB_PATH}"
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${ADDLIB_PATH}"
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${ADDLIB_PATH}"
    )
endforeach()

# 设置C++11

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  

set(CMAKE_CXX_FLAGS_RELEASE "/Zi /MP /Od")
SET(CMAKE_C_FLAGS_RELEASE "/Zi /MP /Od")


message("QTDIR: " ${QTDIR})

set (CMAKE_PREFIX_PATH ${QTDIR})

# find QT dll
# REQUIRED，指示CMake在找不到指定库时报错退出
set(FIND_MODE REQUIRED)
# find_package是CMake用来查找并配置外部项目或库的命令

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
	Network
    LinguistTools
)

#设置头文件一个搜索路径为Src目录
include_directories(src)

#三方库


# 使用QT moc编译
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 添加宏定义
add_compile_options(-D QT_MESSAGELOGCONTEXT=1)

#MainFrame
file(GLOB MAINFRAME_HEADS
    src/MainFrame/*.h
)
aux_source_directory(src/MainFrame MAINFRAME_HEADS_SOURCES)
source_group(src\\MainFrame FILES ${MAINFRAME_HEADS_SOURCES} ${MAINFRAME_HEADS})
list(APPEND ALL_SOURCES_AND_HEADERS ${MAINFRAME_HEADS_SOURCES})
list(APPEND ALL_SOURCES_AND_HEADERS ${MAINFRAME_HEADS})
include_directories(src/MainFrame)

#Login
file(GLOB LOGIN_HEADS
    src/Login/*.h
)
aux_source_directory(src/Login LOGIN_SOURCES)
source_group(src\\Login FILES ${LOGIN_SOURCES} ${LOGIN_HEADS})
list(APPEND ALL_SOURCES_AND_HEADERS ${LOGIN_SOURCES})
list(APPEND ALL_SOURCES_AND_HEADERS ${LOGIN_HEADS})
include_directories(src/Login)

#Net
file(GLOB NET_HEADS
    src/Net/*.h
)
aux_source_directory(src/Net Net_SOURCES)
source_group(src\\Net FILES ${Net_SOURCES} ${NET_HEADS})
list(APPEND ALL_SOURCES_AND_HEADERS ${Net_SOURCES})
list(APPEND ALL_SOURCES_AND_HEADERS ${NET_HEADS})
include_directories(src/Net)

#Log
file(GLOB Log_HEADS
    src/Log/*.h
)
aux_source_directory(src/Log Log_SOURCES)
source_group(src\\Log FILES ${Log_SOURCES} ${Log_HEADS})
list(APPEND ALL_SOURCES_AND_HEADERS ${Log_SOURCES})
list(APPEND ALL_SOURCES_AND_HEADERS ${Log_HEADS})
include_directories(src/Log)

#include
file(GLOB INCLUDE_HEADS
    include/*.h
)
source_group(include FILES ${INCLUDE_HEADS})
list(APPEND ALL_SOURCES_AND_HEADERS ${INCLUDE_HEADS})
include_directories(include)

# 创建ui文件夹 编译打包
set(JQ_UI 
	src/MainFrame/JQChatMainFrame.ui
	src/Login/Login.ui
)
source_group(UI FILES ${JQ_UI})
qt_wrap_ui(LC_UI_HEADERS ${JQ_UI})
source_group(UI_H FILES ${JQ_UI_HEADERS})
#头文件和源文件 统一处理，添加
list(APPEND ALL_SOURCES_AND_HEADERS ${LC_UI_HEADERS})

#main
file(GLOB MAIN_HEADS
    src/*.h
)
aux_source_directory(src/ SRC_SOURCES)
source_group(src\\ FILES  ${SRC_SOURCES})
list(APPEND ALL_SOURCES_AND_HEADERS ${SRC_SOURCES})

#main

set(Net_HEADS
	src/Net/TcpClient.h
	src/Net/TcpClientManager.h
)
aux_source_directory(src/Net Net_SOURCES)
source_group(src\\Net FILES ${Net_SOURCES} ${Net_HEADS})
list(APPEND ALL_SOURCES_AND_HEADERS ${Net_SOURCES})
list(APPEND ALL_SOURCES_AND_HEADERS ${Net_HEADS})
include_directories(src/Net)

# ---------------- 资源文件 ----------------
set(JQ_QRC
    Resources/JQChat.qrc
)

qt_add_resources(JQ_QRC_SOURCES
    ${JQ_QRC}
)

list(APPEND ALL_SOURCES_AND_HEADERS ${JQ_QRC_SOURCES})
source_group("Resource Files" FILES ${JQ_QRC})

# ---------------- 翻译文件 ----------------
qt_add_translations(JQChat
    TS_FILES
        Resources/translates/JQChat.ts
)

set(JQ_TS_FILES
    Resources/translates/JQChat.ts
)

source_group("Translation Files" FILES ${JQ_TS_FILES})



# 设置属性路径
set_property(GLOBAL PROPERTY SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 

# 生成动态库
add_executable(JQChat ${ALL_SOURCES_AND_HEADERS})


# 设置输出路径 废弃。使用这种方式会增加个debug/release
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# 设置所有构建类型输出路径为 bin/
set_target_properties(JQChat PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/bin"
)

#开启生成dbg
if(MSVC)
    target_link_options(JQChat PUBLIC /DEBUG)
	set_target_properties(JQChat PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(ProjectDir)/$(Configuration)")
endif()

if(MSVC)
    set_target_properties(JQChat PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    # 显式设置链接器子系统为 WINDOWS 不显示控制台
    target_link_options(JQChat PRIVATE
        \$<\$<CONFIG:Debug>:/SUBSYSTEM:WINDOWS>
        \$<\$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
        \$<\$<CONFIG:RelWithDebInfo>:/SUBSYSTEM:WINDOWS>
        \$<\$<CONFIG:MinSizeRel>:/SUBSYSTEM:WINDOWS>
    )
endif()

# 关联相关库文件
target_link_libraries(JQChat 
	Qt6::Widgets
	Qt6::Network
	)
