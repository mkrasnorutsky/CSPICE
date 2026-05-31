# CSPICE

NAIF CSPICE toolkit plus AstrologerXP asteroid ephemeris wrappers. Shared C/C++ sources build for **iOS/tvOS/macOS** (SPM xcframework) and **Android** (Maven AAR with JNI).

## Layout

```
common/                 Shared C/C++ (single source of truth)
  include/CSPICE/       Public headers (Asteroids.h, CSPICE.h, AsteroidNames.h)
  src/                  Asteroids.cpp, Directory.cpp, SPKFileInfo.cpp
  CMakeLists.txt        Static library for Android NDK

CSPICE/CSPICE/src/cspice/   NAIF CSPICE C sources (~2150 files)

android/                Gradle library → Maven `co.krasnorutsky:cspice`
ios/                    (Xcode project at repo root)

Package.swift           iOS/tvOS/macOS SPM binary target
build-ios.sh            Builds CSPICE.xcframework
build-android.sh        Builds release AAR
```

## iOS / macOS / tvOS (SPM)

```bash
./build-ios.sh
# dist/CSPICE.xcframework.zip → GitHub release → Package.swift checksum
```

Consumers (Swift):

```swift
.package(url: "https://github.com/mkrasnorutsky/CSPICE", from: "1.0.5")
// import CSPICE
// asteroids_init(docsPath)
```

C/C++ consumers link the xcframework and `#include <CSPICE/Asteroids.h>`.

## Android (Maven)

```bash
./build-android.sh
# android/library/build/outputs/aar/library-release.aar
```

Publish locally:

```bash
cd android
./gradlew :library:publishToMavenLocal
```

Dependency:

```kotlin
implementation("co.krasnorutsky:cspice:1.0.5")
```

Usage:

```java
import co.krasnorutsky.cspice.Cspice;

Cspice.init(getFilesDir().getAbsolutePath());
Cspice.AsteroidResult r = Cspice.calculate(jd, naifId, 0);
if (r.valid) {
    // r.posX … r.velZ in km and km/s
}
int[] loaded = Cspice.loadedSpkIds();
```

Copy SPK/BSP kernel files into the documents directory passed to `init()`.

## Initialization

Call `asteroids_init` / `Cspice.init` once with the app writable directory before any other API. The singleton loads `Asteroids.json` and ephemeris files from that path on first use.

## Version

1.0.5
