# Contributed by Nathan Rusch, Luma Pictures
cmake_minimum_required(VERSION 3.12)

project(HoudiniUsdBridge)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
set(HOUDINI_PATH ""
    CACHE STRING
    "The path to the Houdini installation. This is optional if the HFS environment variable is set.")
set(BOOST_NAMESPACE "boost"
    CACHE STRING
    "The namespace of the Boost build you are using with USD.")
set(USD_LIB_PREFIX "libusd"
    CACHE STRING
    "The name prefix of the USD libraries to build/link against.")
option(COPY_HOUDINI_USD_PLUGINS "Copy $HH/dso/usd_plugins from Houdini to the project installation directory" ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Find Houdini
if (WIN32)
	find_path(HOUDINI_ROOT custom/houdini/dsolib/libFUSE.lib
		HINTS ${HOUDINI_PATH} ENV HFS
		NO_DEFAULT_PATH)
	set(PLATFORM_CXX_OPTIONS "")
	set(PLATFORM_LINK_OPTIONS "")
else()
    find_path(HOUDINI_ROOT dsolib/libHoudiniAPPS3${CMAKE_SHARED_LIBRARY_SUFFIX}
		HINTS ${HOUDINI_PATH} ENV HFS
		NO_DEFAULT_PATH)
	set(PLATFORM_CXX_OPTIONS
        -Wno-missing-field-initializers
        -Wno-unused-function)
	set(PLATFORM_LINK_OPTIONS "-Wl,--exclude-libs,ALL")
endif()

find_path(HOUDINI_INCLUDE_DIR HAPI/HAPI_Version.h
    HINTS ${HOUDINI_PATH} ENV HFS
    PATH_SUFFIXES toolkit/include
    NO_DEFAULT_PATH)

if (NOT (HOUDINI_ROOT AND HOUDINI_INCLUDE_DIR))
    message(FATAL_ERROR
        "Could not find Houdini dirs (HOUDINI_PATH: \"${HOUDINI_PATH}\", HFS: \"$ENV{HFS}\")")
endif()

message(STATUS "Found Houdini at ${HOUDINI_ROOT}")

# Find other dependencies
find_package(USD REQUIRED)

# find_package(Boost
#     COMPONENTS python
#     REQUIRED)

# find_package(PythonInterp 3.9 REQUIRED)
# find_package(PythonLibs 3.9 REQUIRED)

#########################################################
# Marcelo: Code copied from USD package.
#########################################################
option(USE_PYTHON_3 "Use Python 3" ON)

# Find Boost package before getting any boost specific components as we need to
# disable boost-provided cmake config, based on the boost version found.
find_package(Boost REQUIRED)

# Boost provided cmake files (introduced in boost version 1.70) result in
# inconsistent build failures on different platforms, when trying to find boost
# component dependencies like python, program options, etc. Refer some related
# discussions:
# https://github.com/boostorg/python/issues/262#issuecomment-483069294
# https://github.com/boostorg/boost_install/issues/12#issuecomment-508683006
#
# Hence to avoid issues with Boost provided cmake config, Boost_NO_BOOST_CMAKE
# is enabled by default for boost version 1.70 and above. If a user explicitly
# set Boost_NO_BOOST_CMAKE to Off, following will be a no-op.
if (${Boost_VERSION_STRING} VERSION_GREATER_EQUAL "1.70")
    option(Boost_NO_BOOST_CMAKE "Disable boost-provided cmake config" ON)
    if (Boost_NO_BOOST_CMAKE)
        message(STATUS "Disabling boost-provided cmake config")
    endif()
endif()

# --Python.
if(USE_PYTHON_3)
    find_package(PythonInterp 3.0 REQUIRED)
    find_package(PythonLibs 3.0 REQUIRED)
else()
    find_package(PythonInterp 2.7 REQUIRED)
    find_package(PythonLibs 2.7 REQUIRED)
endif()

if (${Boost_VERSION_STRING} VERSION_GREATER_EQUAL "1.67")
    # As of boost 1.67 the boost_python component name includes the
    # associated Python version (e.g. python27, python36).
    # XXX: After boost 1.73, boost provided config files should be able to
    # work without specifying a python version!
    # https://github.com/boostorg/boost_install/blob/master/BoostConfig.cmake

    # Find the component under the versioned name and then set the generic
    # Boost_PYTHON_LIBRARY variable so that we don't have to duplicate this
    # logic in each library's CMakeLists.txt.
    set(python_version_nodot "${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}")
    find_package(Boost
        COMPONENTS
            python${python_version_nodot}
        REQUIRED
    )
    set(Boost_PYTHON_LIBRARY "${Boost_PYTHON${python_version_nodot}_LIBRARY}")
else()
    find_package(Boost
        COMPONENTS
            python
        REQUIRED
    )
endif()
#########################################################
#########################################################

add_subdirectory(src)
