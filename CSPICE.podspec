Pod::Spec.new do |s|
  s.name         = 'CSPICE'
  s.version      = '1.0.1'
  s.summary      = 'NAIF CSPICE toolkit with Asteroids ephemeris API for iOS and macOS.'
  s.description  = <<-DESC
    Binary pod distributing CSPICE as a static XCFramework (iOS device,
    iOS Simulator, macOS universal). Public API: CSPICE module umbrella header
    and C++ Asteroids.h for SPK/JPL asteroid calculations.

    Before publishing to CocoaPods, run `./build.sh` to produce
    `dist/CSPICE.xcframework`, or rely on the prepare_command below.
  DESC

  # Update these URLs before `pod trunk push`.
  s.homepage     = 'https://github.com/mkrasnorutsky/CSPICE.git'
  s.license      = { :type => 'Custom', :file => 'LICENSE' }
  s.author       = { 'Mikhail Krasnorutsky' => 'friendofthai@gmail.com' }
  s.source       = {
    :git => 'https://github.com/mkrasnorutsky/CSPICE.git',
    :tag => "cspice-#{s.version}"
  }

  s.ios.deployment_target  = '12.0'
  s.osx.deployment_target  = '10.13'

  s.vendored_frameworks = 'dist/CSPICE.xcframework'
  s.static_framework      = true

  # Build the XCFramework when installing from a git tag that does not include dist/.
  s.prepare_command = <<-CMD
    set -e
    if [ ! -d "dist/CSPICE.xcframework" ]; then
      ./build.sh
    fi
  CMD

  s.pod_target_xcconfig = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'gnu++14',
    'CLANG_CXX_LIBRARY' => 'libc++',
    'GCC_PREPROCESSOR_DEFINITIONS' => 'CSPICE_SKIP_PLATFORM_CHECK'
  }
  s.user_target_xcconfig = {
    'CLANG_CXX_LANGUAGE_STANDARD' => 'gnu++14',
    'CLANG_CXX_LIBRARY' => 'libc++'
  }

  s.frameworks = 'Foundation'
  s.libraries    = 'c++'
end
