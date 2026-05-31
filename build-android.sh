#!/usr/bin/env bash
#
# Build the Android AAR (release) and publish to the local Maven repo (~/.m2).
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANDROID_DIR="${SCRIPT_DIR}/android"

if [[ ! -d "${ANDROID_DIR}" ]]; then
	echo "error: android project not found" >&2
	exit 1
fi

# macOS ships /usr/bin/java as a stub; Gradle needs a real JDK (17+).
if [[ -z "${JAVA_HOME:-}" ]]; then
	if command -v brew >/dev/null 2>&1; then
		JAVA_HOME="$(brew --prefix openjdk@17 2>/dev/null || true)"
	fi
	if [[ -z "${JAVA_HOME:-}" || ! -x "${JAVA_HOME}/bin/java" ]]; then
		echo "error: JAVA_HOME not set and OpenJDK 17 not found (brew install openjdk@17)" >&2
		exit 1
	fi
	export JAVA_HOME
	export PATH="${JAVA_HOME}/bin:${PATH}"
fi

# Android SDK (Gradle reads ANDROID_HOME or android/local.properties).
if [[ -z "${ANDROID_HOME:-}" && -d "${HOME}/Library/Android/sdk" ]]; then
	export ANDROID_HOME="${HOME}/Library/Android/sdk"
fi
if [[ -z "${ANDROID_HOME:-}" && ! -f "${ANDROID_DIR}/local.properties" ]]; then
	echo "error: set ANDROID_HOME or create android/local.properties (see local.properties.example)" >&2
	exit 1
fi

cd "${ANDROID_DIR}"
./gradlew :library:publishToMavenLocal

GROUP_ID="co.krasnorutsky"
ARTIFACT_ID="cspice"
VERSION="1.0.5"
MAVEN_LOCAL="${HOME}/.m2/repository/co/krasnorutsky/${ARTIFACT_ID}/${VERSION}"

echo "AAR: ${ANDROID_DIR}/library/build/outputs/aar/library-release.aar"
echo "Maven local: ${MAVEN_LOCAL}"
echo "Dependency: implementation(\"${GROUP_ID}:${ARTIFACT_ID}:${VERSION}\")"
