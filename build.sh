#!/usr/bin/env bash
#
# Build CSPICE.xcframework for distribution.
#
# XCFramework slices (3 frameworks — universal macOS + iOS sim, thin iOS device):
#   - macOS:           arm64 + x86_64 (single universal framework)
#   - iOS device:      arm64
#   - iOS simulator:   arm64 + x86_64 (single universal framework)
#
# Usage:
#   ./build.sh              # Release build -> dist/CSPICE.xcframework
#   CONFIGURATION=Debug ./build.sh
#   ./build.sh --clean      # remove build/ and dist/ only
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT="${SCRIPT_DIR}/CSPICE.xcodeproj"
TARGET="CSPICE"
CONFIG="${CONFIGURATION:-Release}"
BUILD_ROOT="${SCRIPT_DIR}/build"
FRAMEWORKS_DIR="${BUILD_ROOT}/frameworks"
OUTPUT_DIR="${SCRIPT_DIR}/dist"
OUTPUT_XCFRAMEWORK="${OUTPUT_DIR}/CSPICE.xcframework"

if [[ "${1:-}" == "--clean" ]]; then
	rm -rf "${BUILD_ROOT}" "${OUTPUT_DIR}"
	echo "Removed ${BUILD_ROOT} and ${OUTPUT_DIR}"
	exit 0
fi

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
	sed -n '2,14p' "$0"
	exit 0
fi

if ! command -v xcodebuild >/dev/null 2>&1; then
	echo "error: xcodebuild not found (install Xcode and select it with xcode-select)" >&2
	exit 1
fi

if [[ ! -d "${PROJECT}" ]]; then
	echo "error: project not found at ${PROJECT}" >&2
	exit 1
fi

log() { printf '==> %s\n' "$*"; }

# sdk archs (space-separated) output-label
build_slice() {
	local sdk="$1"
	local archs="$2"
	local label="$3"

	local symroot="${BUILD_ROOT}/sym/${label}"
	local objroot="${BUILD_ROOT}/obj/${label}"
	local dest="${FRAMEWORKS_DIR}/${label}/CSPICE.framework"

	log "Building ${label} (${sdk} / ${archs})"

	rm -rf "${symroot}" "${objroot}" "${FRAMEWORKS_DIR}/${label}"
	mkdir -p "${symroot}" "${objroot}" "${FRAMEWORKS_DIR}/${label}"

	# Use SYMROOT (not -derivedDataPath): recent xcodebuild requires -scheme with -derivedDataPath.
	local xcodebuild_args=(
		build
		-project "${PROJECT}"
		-target "${TARGET}"
		-configuration "${CONFIG}"
		-sdk "${sdk}"
		ARCHS="${archs}"
		SYMROOT="${symroot}"
		OBJROOT="${objroot}"
		ONLY_ACTIVE_ARCH=NO
		SKIP_INSTALL=NO
		BUILD_LIBRARY_FOR_DISTRIBUTION=YES
		CODE_SIGNING_ALLOWED=NO
		CODE_SIGN_IDENTITY=-
	)

	if [[ "${VERBOSE:-}" != "1" ]]; then
		xcodebuild_args+=(QUIET=YES)
	fi

	xcodebuild "${xcodebuild_args[@]}"

	local built
	built="$(find "${symroot}" -name "${TARGET}.framework" -type d 2>/dev/null | head -1)"
	if [[ -z "${built}" ]]; then
		echo "error: ${TARGET}.framework not found under ${symroot}" >&2
		exit 1
	fi

	# Copy to a stable path for -create-xcframework.
	ditto "${built}" "${dest}"
	xattr -cr "${dest}" 2>/dev/null || true

	log "Built ${dest}"
}

rm -rf "${BUILD_ROOT}" "${OUTPUT_DIR}"
mkdir -p "${FRAMEWORKS_DIR}" "${OUTPUT_DIR}"

build_slice macosx           "arm64 x86_64"  macos
build_slice iphoneos         arm64           ios
build_slice iphonesimulator  "arm64 x86_64"  ios-simulator

log "Creating ${OUTPUT_XCFRAMEWORK}"

xcodebuild -create-xcframework \
	-framework "${FRAMEWORKS_DIR}/macos/CSPICE.framework" \
	-framework "${FRAMEWORKS_DIR}/ios/CSPICE.framework" \
	-framework "${FRAMEWORKS_DIR}/ios-simulator/CSPICE.framework" \
	-output "${OUTPUT_XCFRAMEWORK}"

log "Done: ${OUTPUT_XCFRAMEWORK}"
log "Library identifiers:"
find "${OUTPUT_XCFRAMEWORK}" -mindepth 1 -maxdepth 1 -type d ! -name '.*' -exec basename {} \; \
	| sort | sed 's/^/  /'
