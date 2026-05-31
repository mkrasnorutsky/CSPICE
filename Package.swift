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
            url: "https://github.com/mkrasnorutsky/CSPICE/releases/download/1.0.5/CSPICE.xcframework.zip",
            checksum: "153a574225b913145b1a880ff555a2275b923297248892a5d143725b59083a26"
        )
    ]
)