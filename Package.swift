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
            url: "https://github.com/mkrasnorutsky/CSPICE/releases/download/1.0.9/CSPICE.xcframework.zip",
            checksum: "bf83b38875e30286beff672d3bcc76862ed5ad40f88eaaa24bf0182c30c28791"
        )
    ]
)