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
            url: "https://github.com/mkrasnorutsky/CSPICE/releases/download/1.0.10/CSPICE.xcframework.zip",
            checksum: "9ea7c2bbff01c19e9635481b192893916263c8a2239419e8c5494677a4d07ca4"
        )
    ]
)