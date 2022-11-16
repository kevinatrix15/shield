#!/bin/bash -e

# CONFIG VARIABLES ############################################################

BUILD_DIR=build
CONFIG_FLAGS=""
CMAKE="$(which cmake || true)"

BUILD_TYPE="RELEASE"
TEST=1
COVERAGE=0

# find out how many threads to build with
NTHREADS="1"
if [[ -x "$(which nproc)" ]]; then
  NTHREADS="$(nproc --all)"
else
  NTHREADS="$(cat /proc/cpuinfo | grep -c '^processor')"
fi

# FUNCTIONS ###################################################################

die() {
  echo "ERROR: ${@}" 1>&2
  exit 1
}

show_help() {
  echo "USAGE: build.sh [options]"
  echo ""
  echo "OPTIONS:"
  echo "  --builddir"
  echo "    Set directory to build in."
  echo "  --cc=<cc>"
  echo "    Set the C compiler to use."
  echo "  --cxx=<cxx>"
  echo "    Set the C++ compiler to use."
  echo "  --debug"
  echo "    Build with debugging."
  echo "  -h | --help"
  echo "    Show this help page."
  echo "  -j<n> | --nthreads=<n>"
  echo "    Build using n threads."
  echo "  --verbose"
  echo "    Output each build command."
  echo ""
}



# PARSE OPTIONS ###############################################################


for i in "${@}"; do
  case "${i}" in
    # set build dir
    --builddir=*)
    BUILD_DIR="${i#*=}"
    ;;
    # cc
    --cc=*)
    CONFIG_FLAGS="${CONFIG_FLAGS} -DCMAKE_C_COMPILER=${i#*=}"
    ;;
    # cxx
    --cxx=*)
    CONFIG_FLAGS="${CONFIG_FLAGS} -DCMAKE_CXX_COMPILER=${i#*=}"
    ;;
    # debug
    --debug)
    BUILD_TYPE="DEBUG"
    ;;
    # help
    -h|--help)
    show_help
    exit 0
    ;;
    # j
    -j*)
    NTHREADS="${i#-j}"
    ;;
    # nthreads
    --nthreads=*)
    NTHREADS="${i#*=}"
    ;;
    # verbose
    --verbose)
    CONFIG_FLAGS="${CONFIG_FLAGS} -DCMAKE_VERBOSE_MAKEFILE=1"
    ;;
    # bad argument
    *)
    die "Unknown option '${i}'"
    ;;
  esac
done

# CHECK FOR CMAKE #############################################################

if [[ -z "${CMAKE}" || ! -x "${CMAKE}" ]]; then
  die "Could not find usable cmake: '${CMAKE}'"
else
  echo "Found CMAKE: '${CMAKE}'"
  echo "--> $("${CMAKE}" --version)"
fi

# CREATE BUILD DIRECTORY ######################################################

if [[ -d "${BUILD_DIR}" ]]; then
  /bin/rm -rf "${BUILD_DIR}"
fi
/bin/mkdir -p "${BUILD_DIR}"

# CREATE BUILD ################################################################

pushd "${BUILD_DIR}"

/usr/bin/cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ${CONFIG_FLAGS}

/usr/bin/make -j "${NTHREADS}"

export CTEST_OUTPUT_ON_FAILURE=1
# Run unit tests not categorized as "long-running"
/usr/bin/make test ARGS="-E long_*"
if [[ "${BUILD_TYPE}" == "RELEASE" ]]; then
  # Run long unit tests in release mode only
  /usr/bin/make test ARGS="-R long_*"
fi

popd
