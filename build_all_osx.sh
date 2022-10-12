#!/bin/bash


# --------------------------------------------
#            Utilities

pushdir () {
  command pushd "$@" > /dev/null
}

popdir () {
  command popd "$@" > /dev/null
}

# --------------------------------------------
#         Getting Project Path
#
# Project is stored in PROJECT_PATH

SCRIPT_REL_DIR=$(dirname "${BASH_SOURCE[0]}")
pushdir $SCRIPT_REL_DIR
PROJECT_PATH=$(pwd)
popdir

# --------------------------------------------
#            Compiler Flags

CompilerFlags="-ObjC -fdiagnostics-absolute-paths -O0 -g -DDEBUG=1 -fsanitize=address"
LinkerFlags=""
LinkerLibraries="-framework OpenGL -framework Cocoa -framework AudioToolbox $PROJECT_PATH/examples/libs/freetype/libfreetype-mac.a -I$PROJECT_PATH/examples/libs/freetype/freetype2"

echo $LinkerLibraries
# --------------------------------------------
#            Compiling Samples

if [ ! -d $PROJECT_PATH/build ];
then
  mkdir -p $PROJECT_PATH/build
fi

pushdir $PROJECT_PATH/build

echo Compiling Tests...

clang -o sloth_tests $CompilerFlags ../tests/sloth_tests.c $LinkerLibraries

echo Compiling Examples...

clang -o sloth_sokol_ft_sample $CompilerFlags ../examples/sloth_sokol_ft_sample.c $LinkerLibraries

echo ...Done
echo .

echo Running Tests
./sloth_tests

popdir