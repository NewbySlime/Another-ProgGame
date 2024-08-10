#!/usr/bin/env python
import os
import sys


# User variables
build_path = "editor_workspace/bin"
source_path = "src/src_game/main_program"
godotcpp_path = "godot-cpp-4.1"

env = SConscript("src/src_language/child_program/Sconstruct")
env = SConscript("src/src_language/language_loader/Sconstruct")
env = SConscript(godotcpp_path+"/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=[source_path+"/"])
sources = Glob(source_path+"/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "{}/{}.{}.{}.framework/{}.{}.{}".format(
            build_path, "libgdex", env["platform"], env["target"], "libgdex", env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "{}/{}{}{}".format(
            build_path, "libgdex", env["suffix"], env["SHLIBSUFFIX"]
        ),
        source=sources,
    )

Default(library)
