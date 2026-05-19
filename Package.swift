// swift-tools-version:5.9

import PackageDescription

let package = Package(
    name: "CSPICE",
    platforms: [
        .iOS(.v13),
        .macOS(.v10_15)
    ],
    products: [
        .library(
            name: "CSPICE",
            targets: ["CSPICE"]
        )
    ],
    targets: [
        .binaryTarget(
            name: "CSPICE",
            url: "https://github.com/mkrasnorutsky/CSPICE/releases/download/cspice-1.0.3/CSPICE.xcframework.zip",
            checksum: "625ac10b36a38a2f7657add60badc0853f416fde7c4fc190330ef9c0633b4068"
        )
    ]
)