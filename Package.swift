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
            url: "https://github.com/mkrasnorutsky/CSPICE/releases/download/1.0.6/CSPICE.xcframework.zip",
            checksum: "125ae59030790b28f3d5d6a524de4b67552147bca0d3b5d2f649e769c7f8f22e"
        )
    ]
)