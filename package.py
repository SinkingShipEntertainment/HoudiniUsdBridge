name = "houdiniusdbridge"

authors = [
    "SideFx"
]

# --------------------------------------
# NOTE: The HoudiniUsdBridge repo has branches for major Houdini versions.
# For example, the name of the branch is "houdini19.5".
# Within that branch, each commit has a message that indicates which specific
# Houdini version should be used.
# So, for instance, for Houdini 19.5.368, we need to find the commit message
# for that specific version and work off from there.
#
# Use: git checkout -b <branch_name> <commit-hash>
# Example (to match Houdini 19.5.368):
# git checkout -b houdini19.5 1e7a557202808486e128cfaa054ca6ef13b2e310
# --------------------------------------

# NOTE: version = <bridge_version>.sse.<sse_version>
version = "19.5.368.sse.1.0.0"

description = \
    """
    USD plugins for Houdini
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
    "houdini-19.5.368",
]

private_build_requires = [
]

variants = [
    #["platform-linux", "arch-x86_64", "os-centos-7", "usd-21.08"],
    ["platform-linux", "arch-x86_64", "os-centos-7", "usd-22.11"],
]

# Pass cmake arguments:
# rez-build -i -- -DBoost_NO_SYSTEM_PATHS=True
# rez-release -- -DBoost_NO_SYSTEM_PATHS=True

uuid = "repository.HoudiniUsdBridge"

def pre_build_commands():
    command("source /opt/rh/devtoolset-7/enable")  # Using devtoolset-7

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

    env.HOUDINIUSDBRIDGE_ROOT = "{root}"
    env.HOUDINIUSDBRIDGE_LOCATION = "{root}"

def post_commands():
    # From the Bridge docs
    env.BRIDGE_ROOT = "{root}"
    env.LD_LIBRARY_PATH.append("{root}/dsolib")
    env.PYTHONPATH.prepend("{root}/houdini/python3.7libs")
    env.HOUDINI_PATH.append("{root}/houdini;&")
    env.HOUDINI_USD_DSO_PATH = "{root}/houdini/dso/usd_plugins"
    env.HOUDINI_DSO_EXCLUDE_PATTERN = "{$HH/dso/USD_Ops.so,$HH/dso/fs/USD_FS.so}"

    env.PXR_PLUGINPATH_NAME.append("{root}/houdini/dso/usd_plugins")
    env.PXR_PLUGINPATH_NAME.append("$HDSO/usd_plugins")
    env.PXR_PLUGINPATH_NAME.append("$HH/dso/usd_plugins")