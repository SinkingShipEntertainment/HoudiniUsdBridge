name = "houdiniusdbridge"

authors = [
    "SideFx"
]

# NOTE: version = <houdini_version>.sse.<python_major><sse_version>
version = "18.5.596.sse.2.0.0"

description = \
    """
    USD Houdini plugins
    """

with scope("config") as c:
    # Determine location to release: internal (int) vs external (ext)

    # NOTE: Modify this variable to reflect the current package situation
    release_as = "ext"

    # The `c` variable here is actually rezconfig.py
    # `release_packages_path` is a variable defined inside rezconfig.py

    import os
    if release_as == "int":
        c.release_packages_path = os.environ["SSE_REZ_REPO_RELEASE_INT"]
    elif release_as == "ext":
        c.release_packages_path = os.environ["SSE_REZ_REPO_RELEASE_EXT"]

    #c.build_thread_count = "physical_cores"

# NOTE: boost will determine the python version to use
requires = [
    "houdini-18.5.596.sse.2",
]

private_build_requires = [
]

variants = [
    ["platform-linux", "arch-x86_64", "os-centos-7", "boost-1.61.0", "!ptex", "usd-21.05"],
]

# If want to use Ninja, run:
# rez-build -i --cmake-build-system "ninja"
# rez-release --cmake-build-system "ninja"
#
# Pass cmake arguments:
# rez-build -i -- -DBoost_NO_BOOST_CMAKE=On -DBoost_NO_SYSTEM_PATHS=True
# rez-release -- -DBoost_NO_BOOST_CMAKE=On -DBoost_NO_SYSTEM_PATHS=True

uuid = "repository.HoudiniUsdBridge"

def pre_build_commands():
    command("source /opt/rh/devtoolset-6/enable")

def commands():
    # NOTE: REZ package versions can have ".sse." to separate the external
    # version from the internal modification version.
    split_versions = str(version).split(".sse.")
    external_version = split_versions[0]
    internal_version = None
    if len(split_versions) == 2:
        internal_version = split_versions[1]

    env.HOUDINIUSDBRIDGE_VERSION = external_version
    env.HOUDINIUSDBRIDGE_PACKAGE_VERSION = external_version
    if internal_version:
        env.HOUDINIUSDBRIDGE_PACKAGE_VERSION = internal_version

    env.HOUDINIUSDBRIDGE_ROOT.append("{root}")
    env.HOUDINIUSDBRIDGE_LOCATION.append("{root}")
