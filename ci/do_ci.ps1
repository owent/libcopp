$PSDefaultParameterValues['*:Encoding'] = 'UTF-8'

$OutputEncoding = [System.Text.UTF8Encoding]::new()

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition
$WORK_DIR = Get-Location

if ($IsWindows) {
  # See https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
  New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" `
    -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force

  function Invoke-Environment {
    param
    (
      [Parameter(Mandatory = $true)]
      [string] $Command
    )
    cmd /c "$Command > nul 2>&1 && set" | . { process {
        if ($_ -match '^([^=]+)=(.*)') {
          [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        }
      } }
  }
  $vswhere = "${Env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
  $vsInstallationPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
  $winSDKDir = $(Get-ItemPropertyValue -Path "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" -Name "InstallationFolder")
  if ([string]::IsNullOrEmpty($winSDKDir)) {
    $winSDKDir = "${Env:ProgramFiles(x86)}/Windows Kits/10/Include/"
  }
  else {
    $winSDKDir = "$winSDKDir/Include/"
  }
  foreach ($sdk in $(Get-ChildItem $winSDKDir | Sort-Object -Property Name)) {
    if ($sdk.Name -match "[0-9]+\.[0-9]+\.[0-9\.]+") {
      $selectWinSDKVersion = $sdk.Name
    }
  }
  if (!(Test-Path Env:WindowsSDKVersion)) {
    $Env:WindowsSDKVersion = $selectWinSDKVersion
  }
  # Maybe using $selectWinSDKVersion = "10.0.18362.0" for better compatible
  Write-Output "Window SDKs:(Latest: $selectWinSDKVersion)"
  foreach ($sdk in $(Get-ChildItem $winSDKDir | Sort-Object -Property Name)) {
    Write-Output "  - $sdk"
  }
}

Set-Location "$SCRIPT_DIR/.."
$RUN_MODE = $args[0]

if ( $RUN_MODE -eq "msvc.2019.test" ) {
  Invoke-Environment "call ""$vsInstallationPath/VC/Auxiliary/Build/vcvars64.bat"""
  New-Item -Path "build_jobs_ci" -ItemType "directory" -Force 
  Set-Location "build_jobs_ci"
  & cmake ".." "-G" "$Env:CMAKE_GENERATOR" "-A" $Env:CMAKE_PLATFORM "-DBUILD_SHARED_LIBS=$ENV:BUILD_SHARED_LIBS"  `
    "-DPROJECT_ENABLE_UNITTEST=ON" "-DPROJECT_ENABLE_SAMPLE=ON"                                                   `
    "-DCMAKE_SYSTEM_VERSION=$selectWinSDKVersion" "-DLIBCOPP_FCONTEXT_USE_TSX=ON"                                 `
    "-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_LOW_MEMORY_MODE=ON"
  if ( $LastExitCode -ne 0 ) {
    exit $LastExitCode
  }

  $ALL_DLL_FILES = Get-ChildItem -Path "../third_party/install/*.dll" -Recurse
  $ALL_DLL_DIRS = $(foreach ($dll_file in $ALL_DLL_FILES) {
      $dll_file.Directory.FullName
    }) | Sort-Object | Get-Unique
  $Env:PATH = ($ALL_DLL_DIRS + $Env:PATH) -Join [IO.Path]::PathSeparator

  & cmake --build . --config $Env:CONFIGURATION
  if ( $LastExitCode -ne 0 ) {
    exit $LastExitCode
  }

  & cmake --build . --config RelWithDebInfo --target run_sample
  & cmake --build . --config RelWithDebInfo --target run_test
  & cmake --build . --config RelWithDebInfo --target benchmark
}
elseif ( $RUN_MODE -eq "msvc.2017.test" ) {
  Invoke-Environment "call ""$vsInstallationPath/VC/Auxiliary/Build/vcvars64.bat"""
  New-Item -Path "build_jobs_ci" -ItemType "directory" -Force 
  Set-Location "build_jobs_ci"
  & cmake ".." "-G" "$Env:CMAKE_GENERATOR" "-DBUILD_SHARED_LIBS=$Env:BUILD_SHARED_LIBS"       `
    "-DPROJECT_ENABLE_UNITTEST=ON" "-DPROJECT_ENABLE_SAMPLE=ON" "-DPROJECT_ENABLE_TOOLS=ON"   `
    "-DCMAKE_SYSTEM_VERSION=$selectWinSDKVersion" "-DLIBCOPP_FCONTEXT_USE_TSX=ON"             `
    "-DATFRAMEWORK_CMAKE_TOOLSET_THIRD_PARTY_LOW_MEMORY_MODE=ON"
  if ( $LastExitCode -ne 0 ) {
    exit $LastExitCode
  }

  $ALL_DLL_FILES = Get-ChildItem -Path "../third_party/install/*.dll" -Recurse
  $ALL_DLL_DIRS = $(foreach ($dll_file in $ALL_DLL_FILES) {
      $dll_file.Directory.FullName
    }) | Sort-Object | Get-Unique
  $Env:PATH = ($ALL_DLL_DIRS + $Env:PATH) -Join [IO.Path]::PathSeparator

  & cmake --build . --config $Env:CONFIGURATION
  if ( $LastExitCode -ne 0 ) {
    exit $LastExitCode
  }

  & cmake --build . --config $Env:CONFIGURATION --target run_sample
  & cmake --build . --config $Env:CONFIGURATION --target run_test
  & cmake --build . --config $Env:CONFIGURATION --target benchmark
}

Set-Location $WORK_DIR
