#!/bin/bash

#Daswf852's build script for his projects
#Feel free to do whatever you want with this

function fail() {
    if (( $# == 1 )); then
        echo "Build failed: $1"
        exit 1
    elif (( $# == 2 )); then
        echo "Build failed: $1"

        if ! [[ "$2" =~ '^[0-9]+$' ]]; then
            exit $2
        else
            exit 1
        fi
    else
        echo "Build failed."
        exit 1
    fi
    
}

function printUsage() {
    echo "Possible arguments:"
    echo "-j: GNU Make job count"
    echo "Accepts: any numerical value"
    echo "-b: CMake build type"
    echo "Accepts: anything, CMake might complain depending on the input"
    echo "-ca: Additional arguments for CMake"
    echo "Accepts: anything, put in quotes if you have spaces (duh)"
    echo "-ma: Additional arguments for Make"
    echo "Accepts: same as -ca"
    echo "-fe: Command to be executed after finishing"
    echo "Accepts: some valid bash line i guess"
    echo ""
    echo "Example usage: ./build.sh -b=Release -c=8"
}

let jobCount=1
buildType="Debug"
cArgs=""
mArks=""
fExec=""

## SWITCHES
# -b -> CMake build type
# can be one of: Debug Release
# -j -> GNU Make job count
# can be of: ^[0-9]+$

for arg in "$@"; do
    argValue="${arg#*=}"
    case $arg in
        -b=*)
            buildType="$argValue"
        ;;

        -j=*)
            let jobCount="$argValue" || fail "Bad job count"
        ;;

        -ca=*)
            cArgs="$argValue"
        ;;

        -ma=*)
            mArgs="$argValue"
        ;;

        *)
            printUsage
            fail "Unknown argument got passed: $arg"
        ;;
    esac
done

echo "Build type: $buildType"
echo "Job count: $jobCount"

[ -d build ] || mkdir build || fail "Couldn't create a build directory"
cd build
cmake -DCMAKE_BUILD_TYPE=$buildType -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $cArgs .. || fail "CMake failed"
mv compile_commands.json ..
make -j$jobCount $mArgs

$fExec