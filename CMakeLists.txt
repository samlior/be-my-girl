cmake_minimum_required(VERSION 3.1)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(superminer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

string(TIMESTAMP VERSION_YEAR "%Y")
string(TIMESTAMP VERSION_MONTH_YEAR "%m")
string(TIMESTAMP VERSION_DAY_YEAR "%d")
string(TIMESTAMP HOUR24 "%H")
string(TIMESTAMP MINUTE "%M")

math(EXPR VERSION_MAJOR "${VERSION_YEAR}")
math(EXPR VERSION_MINOR "${VERSION_MONTH_YEAR}")
math(EXPR VERSION_TINY "${VERSION_DAY_YEAR}")
math(EXPR VERSION_BUILD "${HOUR24}*60+${MINUTE}")

include(CheckCXXCompilerFlag)
include(CheckLibraryExists)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/cmake")

option(ENABLE_BUILD_WERROR "all warnings being treated as errors" ON)
option(ENABLE_SYSTEM_OPENSSL "link to system openssl" OFF)
option(ENABLE_USE_GIT_VERSION "use git version" OFF)

option(ETHASHCL "Build with OpenCL mining" ON)
option(ETHASHCUDA "Build with CUDA mining" OFF)
option(ETHASHCPU "Build with CPU mining (only for development)" OFF)
option(ENABLE_LLD "enable lld linker" OFF)

if (ENABLE_LLD)
	set (CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (ETHASHCL)
    add_definitions(-DETH_ETHASHCL)
endif()
if (ETHASHCUDA)
    add_definitions(-DETH_ETHASHCUDA)
endif()
if (ETHASHCPU)
    add_definitions(-DETH_ETHASHCPU)
endif()

if (ENABLE_USE_GIT_VERSION)
	find_package(Git)
	gitGetVersion(${CMAKE_CURRENT_SOURCE_DIR} superpool)
	set(VERSION_GIT ${superpool_WC_REVISION_HASH})
	string(TIMESTAMP PVERSION "%Y%m%d-${VERSION_GIT}-${CMAKE_SYSTEM_NAME}")
endif()

if (MSVC)
	set(CompilerFlags
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_MINSIZEREL
			CMAKE_CXX_FLAGS_RELWITHDEBINFO
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_MINSIZEREL
			CMAKE_C_FLAGS_RELWITHDEBINFO
		)

	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_WIN32_WINNT=0x0601)
	add_definitions(-DBOOST_ALL_STATIC_LINK)
	add_definitions(-DBOOST_THREAD_USE_LIB)
	add_definitions(-DBOOST_FILESYSTEM_STATIC_LINK)
	add_definitions(-DBOOST_USE_WINAPI_VERSION=0x0601)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DNOMINMAX)
	add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj")
	set(CMAKE_CXX_STACK_SIZE "20000000")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DENABLE_LOGGER -DDISABLE_LOGGER_TO_CONSOLE -DLOGGER_DBG_VIEW -DDISABLE_WRITE_LOGGING")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DENABLE_LOGGER -DDISABLE_LOGGER_TO_CONSOLE -DLOGGER_DBG_VIEW -DDISABLE_WRITE_LOGGING")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DENABLE_LOGGER -DDISABLE_LOGGER_TO_CONSOLE -DLOGGER_DBG_VIEW -DDISABLE_WRITE_LOGGING")

	link_libraries(opengl32.lib imm32.lib ws2_32.lib Wtsapi32.lib winmm.lib shell32.lib uxtheme.lib dwmapi.lib Crypt32.lib netapi32.lib Userenv.lib)
endif()

CHECK_CXX_COMPILER_FLAG(-fvisibility-inlines-hidden COMPILER_HAS_VISIBILITY_INLINE_HIDDEN)
CHECK_CXX_COMPILER_FLAG(-fvisibility=hidden COMPILER_HAS_VISIBILITY_HIDDEN)
CHECK_CXX_COMPILER_FLAG(-std=c++14 LIBCXX_HAS_STDCXX14_FLAG)
CHECK_CXX_COMPILER_FLAG(-std=c++17 LIBCXX_HAS_STDCXX17_FLAG)
CHECK_CXX_COMPILER_FLAG(-std=c++20 LIBCXX_HAS_STDCXX20_FLAG)
CHECK_CXX_COMPILER_FLAG(-fdiagnostics-color=always COMPILER_HAS_COLOR)

if (${COMPILER_HAS_VISIBILITY_INLINE_HIDDEN})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

if (${COMPILER_HAS_VISIBILITY_HIDDEN})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if (${COMPILER_HAS_COLOR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

if (NOT MSVC)
	if(NOT APPLE)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	if(LIBCXX_HAS_STDCXX20_FLAG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")
		add_definitions(-DUSE_STD_STRING_VIEW)
	elseif(LIBCXX_HAS_STDCXX17_FLAG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")
		add_definitions(-DUSE_STD_STRING_VIEW)
	elseif(LIBCXX_HAS_STDCXX14_FLAG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")
	else()
		message(FATAL_ERROR "need at least GCC 5 or clang 5")
	endif()
endif(NOT MSVC)

#add_definitions(-DJSONRPC_DISABLE_THREADS)
#add_subdirectory(third_party/crypto)

set(Boost_USE_STATIC_LIBS	ON)
set(Boost_USE_STATIC_RUNTIME	ON)

find_package(Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

set(BOOST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost)

add_definitions(-DBOOST_LOCALE_HIDE_AUTO_PTR)
add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING)
add_subdirectory(third_party/boost)

# add jsoncpp
add_subdirectory(third_party/json)
set(JSONCPP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include/json)

# add zip
add_subdirectory(third_party/zlib EXCLUDE_FROM_ALL)
set(ZLIB_LIBRARIES zlib)
set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib ${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib/contrib)

# add fmt
add_definitions(-DFMT_HEADER_ONLY)
include_directories(third_party/fmt)

# add tinyxml2
add_subdirectory(third_party/tinyxml2)
set(TINYXML2_LIBRARIES tinyxml2)
set(TINYXML2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyxml2)

# add ethash
add_subdirectory(third_party/ethash)
set(ETHASH_LIBRARIES ethash)
set(ETHASH_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ethash/include ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ethash/lib/ethash)

set(THIRD_PARTY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

include_directories(
	${JSONCPP_INCLUDE_DIRS}
	${THIRD_PARTY_DIRS}
	${BOOST_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIRS}
	${TINYXML2_INCLUDE_DIRS}
	${ETHASH_INCLUDE_DIRS}
)

# add opencl
if(MSVC)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencl/include)
	link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencl/lib/x86_64/OpenCL.lib)
else()
	find_package(OpenCL REQUIRED)
	link_libraries(OpenCL::OpenCL)
endif()

if (ENABLE_SYSTEM_OPENSSL)
	find_package(OpenSSL)
else()
	add_subdirectory(third_party/openssl)
	if (NOT MSVC AND NOT APPLE)
		set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--exclude-libs,ALL ${CMAKE_SHARED_LINKER_FLAGS}")
		set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--exclude-libs,ALL ${CMAKE_MODULE_LINKER_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "-Wl,--exclude-libs,ALL ${CMAKE_EXE_LINKER_FLAGS}")
	endif()
endif()

include_directories(${OPENSSL_INCLUDE_DIR})

# add libethcore
add_subdirectory(third_party/libethcore)
set(LIBETHCORE_LIBRARIES ethcore)
set(LIBETHCORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libethcore)

set_target_properties(${LIBETHCORE_LIBRARIES} PROPERTIES FOLDER "third_party")
set_target_properties(${LIBETHCORE_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# add libdevcore
add_subdirectory(third_party/libdevcore)
set(LIBDEVCORE_LIBRARIES devcore)
set(LIBDEVCORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libdevcore)

set_target_properties(${LIBDEVCORE_LIBRARIES} PROPERTIES FOLDER "third_party")
set_target_properties(${LIBDEVCORE_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# add libhwmon
add_subdirectory(third_party/libhwmon)
set(HWMON_LIBRARIES hwmon)
set(HWMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libhwmon)

set_target_properties(${HWMON_LIBRARIES} PROPERTIES FOLDER "third_party")
set_target_properties(${HWMON_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# add libethash-cl
add_subdirectory(third_party/libethash-cl)
set(ETHASH_CL_LIBRARIES ethash-cl)
set(ETHASH_CL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libethash-cl)

set_target_properties(${ETHASH_CL_LIBRARIES} PROPERTIES FOLDER "third_party")
set_target_properties(${ETHASH_CL_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(clbin2h_stable PROPERTIES FOLDER "third_party")

if(ETHASHCPU)
	# add libethash-cpu
	add_subdirectory(third_party/libethash-cpu)
	set(ETHASH_CPU_LIBRARIES ethash-cpu)
	set(ETHASH_CPU_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libethash-cpu)

	set_target_properties(${ETHASH_CPU_LIBRARIES} PROPERTIES FOLDER "third_party")
	set_target_properties(${ETHASH_CPU_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(ETHASHCUDA)
	# add libethash-cuda
	add_subdirectory(third_party/libethash-cuda)
	set(ETHASH_CUDA_LIBRARIES ethash-cuda)
	set(ETHASH_CUDA_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libethash-cuda)

	set_target_properties(${ETHASH_CUDA_LIBRARIES} PROPERTIES FOLDER "third_party")
	set_target_properties(${ETHASH_CUDA_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# add ethminer
# add_subdirectory(third_party/ethminer)
#set(ETHMINER_LIBRARIES ethminer)
#set(ETHMINER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ethminer)

#set_target_properties(${ETHMINER_LIBRARIES} PROPERTIES FOLDER "third_party")
#set_target_properties(${ETHMINER_LIBRARIES} PROPERTIES POSITION_INDEPENDENT_CODE ON)



include_directories(
	${LIBETHCORE_INCLUDE_DIRS}
	${LIBDEVCORE_INCLUDE_DIRS}
	${HWMON_INCLUDE_DIRS}

	${ETHASH_CL_INCLUDE_DIRS}
	${ETHASH_CPU_INCLUDE_DIRS}
	${ETHASH_CUDA_INCLUDE_DIRS}
	${ETHMINER_INCLUDE_DIRS}
)

link_libraries(
		Boost::coroutine
		Boost::context
		Boost::thread
		Boost::date_time
		Boost::filesystem
		Boost::regex
		jsoncpp

		${OPENSSL_LIBRARIES}
		${ZLIB_LIBRARIES}
		${TINYXML2_LIBRARIES}
		${LIBETHCORE_LIBRARIES}

		${ETHASH_LIBRARIES}
		${LIBDEVCORE_LIBRARIES}
		${HWMON_LIBRARIES}

		${ETHASH_CL_LIBRARIES}
		${ETHASH_CPU_LIBRARIES}
		${ETHASH_CUDA_LIBRARIES}
		${ETHMINER_LIBRARIES}
)

if (WIN32)
	link_libraries(
		Secur32.lib
	)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
endif()

add_subdirectory(superminer)
