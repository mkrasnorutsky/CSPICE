// swift-tools-version:5.9

import PackageDescription

let package = Package(
    name: "CSPICE",
    platforms: [
        .iOS(.v13),
        .tvOS(.v15),
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
            url: "https://github.com/mkrasnorutsky/CSPICE/releases/download/1.0.8/CSPICE.xcframework.zip",
            checksum: "dba1c6fed61eb862f4134c2e8a35ff7dd0114eba4acbcca79e4fe4ac67b05d1d"
        )
    ]
)