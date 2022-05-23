<#
.SYNOPSIS
    .
.PARAMETER Help
    Print help message and exit
.PARAMETER BuildDirectory
    Build directory
.PARAMETER BuildType
    Build Type: Debug, Release, RelWithDebInfo, MinSizeRel
.PARAMETER Compiler
    Compiler to use: clang, gcc, /usr/bin/gcc and etc.
.PARAMETER GeneratorToolset
    Generator Toolset of cmake: v143, v142, v141, v140 and etc.
.PARAMETER Generator
    Generator to use: Ninja, Makefiles, Xcode, Visual Studio 16 2019 and etc.
.PARAMETER GeneratorPlatform
    Generator Platform: x64, x86, Win64, armv7s, Win32 and etc.
.PARAMETER VcVarArchitecture
    Argument passed to when run vcvarsall.bat: x64, x86, amd64, amd64_arm64 and etc.
.PARAMETER Distcc
    Path of distcc
.PARAMETER Ccache
    Path of ccache
.PARAMETER Ninja
    Path of Ninja
.PARAMETER EnableSample
    Enable Sample: yes|no|true|false|1|0
.PARAMETER EnableTest
    Enable Test: yes|no|true|false|1|0
.PARAMETER EnableTools
    Enable Tools: yes|no|true|false|1|0
.PARAMETER CMakeOptions
    CMake options(pwsh -File <this file> [options...] --% [cmake options])
.EXAMPLE
    .
    pwsh -File <this file> [options...] --% -DOTHER_CMAKE_OPTIONS1=VALUE -DOTHER_CMAKE_OPTIONS2=VALUE
    Get-Help <this file> -detailed
    Get-Help <this file> -full
#>

[CmdletBinding()]
param (
  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [switch]$Help,

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$BuildDirectory = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
  [string]$BuildType = "Debug",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$Compiler = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$GeneratorToolset = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true,
    HelpMessage = "Generator to use")]
  [string]$Generator = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$GeneratorPlatform = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [ValidateSet('', 'x64', 'amd64', 'x86', 'x86_amd64', 'x86_x64', 'x86_arm', 'x86_arm64', 'amd64_x86', 'x64_x86', 'amd64_arm', 'x64_arm', 'amd64_arm64', 'x64_arm64')]
  [string]$VcVarArchitecture = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$Distcc = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$CCache = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$Ninja = "",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$EnableSample = "true",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$EnableTest = "true",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true)]
  [string]$EnableTools = "true",

  [Parameter(ValueFromPipeline = $true,
    ValueFromPipelineByPropertyName = $true,
    ValueFromRemainingArguments = $true,
    HelpMessage = "CMake options(pwsh -File <this file> [options...] --% [cmake options])")]
  [string[]]$CMakeOptions
)

$PSDefaultParameterValues['*:Encoding'] = 'UTF-8'

$OutputEncoding = [System.Text.UTF8Encoding]::new()

if ($Help) {
  ((Get-Command Get-PowerShellLog).ParameterSets.Parameters |
  Where-Object Name -eq InstanceId).HelpMessage
  exit 0
}

function Test-RegistryValue {
  param(
    [Alias("PSPath")]
    [Parameter(Position = 0, Mandatory = $true, ValueFromPipeline = $true, ValueFromPipelineByPropertyName = $true)]
    [String]$Path
    ,
    [Parameter(Position = 1, Mandatory = $true)]
    [String]$Name
    ,
    [Switch]$PassThru
  ) 

  process {
    if (Test-Path $Path) {
      $Key = Get-Item -LiteralPath $Path
      if ($null -ne $Key.GetValue($Name, $null)) {
        if ($PassThru) {
          Get-ItemPropertyValue -Path $Path -Name $Name
        }
        else {
          $true
        }
      }
      else {
        $false
      }
    }
    else {
      $false
    }
  }
}
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

function Check-BooleanString {
  param
  (
    [Parameter(Position = 0, Mandatory = $true, ValueFromPipeline = $true, ValueFromPipelineByPropertyName = $true)]
    [string]$CheckString
  )

  if ($CheckString.Length -eq 0) {
    return $false
  }

  return !($CheckString.ToLower() -match '^(no|disable|disabled|false|0)$')
}

$VsInstallationPath = $null
if ($Compiler.Length -eq 0) {
  if ($IsMac) {
    $Compiler = "clang";
  }
  elseif ($IsLinux) {
    $Compiler = "gcc";
  }
  elseif ($IsWindows) {
    if ([string]::IsNullOrEmpty($Env:VSINSTALLATIONPATH)) {
      $VsWhere = "${Env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
      $VsInstallationPath = & $VsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    }
    else {
      $VsInstallationPath = $Env:VSINSTALLATIONPATH
    }

    if ($VcVarArchitecture.Length -eq 0) {
      $VcVarArchitecture = "x64"
    }
    Write-Output "VsInstallationPath: $VsInstallationPath"
  }
}

$NinjaOverwriteMakeProgram = $false
if ($Generator.Length -eq 0) {
  $Ninja = Get-Command ninja
  if ($IsWindows) {
    if ($VsInstallationPath -match '[\\/]2022[\\/][\d\w\s_\-\.]+[\\/]?') {
      $Generator = "Visual Studio 17 2022"
      if ($GeneratorPlatform.Length -eq 0) {
        $GeneratorPlatform = "x64"
      }
    }
    elseif ($VsInstallationPath -match '[\\/]2019[\\/][\d\w\s_\-\.]+[\\/]?') {
      $Generator = "Visual Studio 16 2019"
      if ($GeneratorPlatform.Length -eq 0) {
        $GeneratorPlatform = "x64"
      }
    }
    elseif ($VsInstallationPath -match '[\\/]2017[\\/][\d\w\s_\-\.]+[\\/]?') {
      $Generator = "Visual Studio 15 2017 Win64"
    }
  }
  elseif ((Check-BooleanString "$Ninja")) {
    if ($Ninja.Length -eq 0) {
      $Ninja = Get-Command ninja
      if ($? -and $Ninja) {
        $Generator = "Ninja";
      }
    }
    else {
      $Generator = "Ninja";
      $NinjaOverwriteMakeProgram = $true
    }
  }
}

if ($Generator.Length -gt 0) {
  $CMakeGeneratorArgs = @("-G", $Generator) 
}
else {
  $CMakeGeneratorArgs = @() 
}

if ($GeneratorPlatform.Length -gt 0) {
  $CMakeGeneratorArgs += @("-A", $GeneratorPlatform) 
}

if ($GeneratorToolset.Length -gt 0) {
  $CMakeGeneratorArgs += @("-T", $GeneratorToolset) 
}

if ($Compiler.Length -gt 0) {
  $LastSearchGccIndex = $Compiler.LastIndexOf("gcc")
  $LastSearchClangIndex = $Compiler.LastIndexOf("clang")
  if ($LastSearchGccIndex -ge 0) {
    $CompilerCXX = $Compiler.Substring(0, $LastSearchGccIndex) + "g++" + $Compiler.Substring($LastSearchGccIndex + 3);
  }
  elseif ($LastSearchClangIndex -ge 0) {
    $CompilerCXX = $Compiler.Substring(0, $LastSearchClangIndex) + "clang++" + $Compiler.Substring($LastSearchClangIndex + 5);
  }
  else {
    $CompilerCXX = $Compiler;
  }

  $CMakeGeneratorArgs += @("-DCMAKE_C_COMPILER=$Compiler", "-DCMAKE_CXX_COMPILER=$CompilerCXX")
}

if ($Compiler -match "clang") {
  if ((Check-BooleanString "$Distcc")) {
    if ($Distcc.Length -eq 0) {
      $Distcc = Get-Command distcc
    }
  }

  if ((Check-BooleanString "$CCache")) {
    if ($CCache.Length -eq 0) {
      $CCache = Get-Command ccache
    }
  }

  if (($Distcc.Length -gt 0) -and (Test-Path $Distcc)) {
    $CMakeCompilerArgs += @("-DCMAKE_C_COMPILER_LAUNCHER=$Distcc", "-DCMAKE_CXX_COMPILER_LAUNCHER=$Distcc")
  }

  if (($CCache.Length -gt 0) -and (Test-Path $CCache)) {
    $CMakeCompilerArgs += @("-DCMAKE_C_COMPILER_LAUNCHER=$CCache", "-DCMAKE_CXX_COMPILER_LAUNCHER=$CCache")
  }
}

if ($IsWindows) {
  $WinSDKDir = ""
  $WinSDKVersion = [System.Version]::new("0.0")
  foreach ($WinSDKVer in Get-ChildItem -Path "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows") {
    $CurrentWinSDKDir = Test-RegistryValue -Path "Registry::$WinSDKVer" -Name "InstallationFolder" -PassThru
    $CurrentWinSDKVersion = Test-RegistryValue -Path "Registry::$WinSDKVer" -Name "ProductVersion" -PassThru
    if ((-Not $CurrentWinSDKVersion) -or (-Not $CurrentWinSDKDir)) {
      continue
    }
    $CurrentWinSDKVersionObject = [System.Version]::new($CurrentWinSDKVersion)
    if ($CurrentWinSDKVersionObject -gt $WinSDKVersion) {
      $WinSDKVersion = $CurrentWinSDKVersionObject
      $WinSDKDir = $CurrentWinSDKDir
    }
  }

  if ([string]::IsNullOrEmpty($WinSDKDir)) {
    foreach ($item in @("${Env:ProgramFiles(x86)}/Windows Kits/11/Include/",
        "${Env:ProgramFiles(x86)}/Windows Kits/10/Include/",
        "${Env:ProgramFiles(x86)}/Windows Kits/8.1/Include/")) {
      if (Test-Path $item) {
        $WinSDKIncludeDir = $item
        break
      }
    }
  }
  else {
    $WinSDKIncludeDir = "$WinSDKDir/Include/"
  }
  foreach ($sdk in $(Get-ChildItem $WinSDKIncludeDir | Sort-Object -Property Name)) {
    if ($sdk.Name -match "[0-9]+\.[0-9]+\.[0-9\.]+") {
      $SelectWinSDKVersion = $sdk.Name
    }
  }
  if (!(Test-Path Env:WindowsSDKVersion)) {
    $Env:WindowsSDKVersion = $SelectWinSDKVersion
  }
  # Maybe using $SelectWinSDKVersion = "10.0.18362.0" for better compatible
  Write-Output "Window SDKs:(Latest: $SelectWinSDKVersion)"
  foreach ($sdk in $(Get-ChildItem $WinSDKIncludeDir | Sort-Object -Property Name)) {
    Write-Output "  - $sdk"
  }

  $CMakeGeneratorArgs += @("-DCMAKE_SYSTEM_VERSION=$SelectWinSDKVersion") 
}

if (($null -ne $VsInstallationPath) -and ($VcVarArchitecture.Length -gt 0)) {
  Write-Output "call ""$VsInstallationPath/VC/Auxiliary/Build/vcvarsall.bat"" ""$VcVarArchitecture"""
  Invoke-Environment "call ""$VsInstallationPath/VC/Auxiliary/Build/vcvarsall.bat"" ""$VcVarArchitecture"""
}

if ($BuildDirectory.Length -eq 0) {
  $BuildDirectorySystemSuffix = [System.Environment]::OSVersion.Platform.ToString().ToLower()
  $BuildDirectory = "build_jobs_$BuildDirectorySystemSuffix"
}

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$RunCMakeDir = ($ScriptDir, $BuildDirectory) -Join [IO.Path]::DirectorySeparatorChar

if ($Generator.Equals("Ninja") -and $NinjaOverwriteMakeProgram) {
  $CMakeCompilerArgs += @("-DCMAKE_MAKE_PROGRAM=$Ninja")
}

$CMakeGeneratorArgs += @("-DCMAKE_BUILD_TYPE=$BuildType")
if ((Check-BooleanString "$EnableSample")) {
  $CMakeGeneratorArgs += @("-DPROJECT_ENABLE_SAMPLE=ON")
}
if ((Check-BooleanString "$EnableTest")) {
  $CMakeGeneratorArgs += @("-DPROJECT_ENABLE_UNITTEST=ON", "-DBUILD_TESTING=ON")
}
if ((Check-BooleanString "$EnableTools")) {
  $CMakeGeneratorArgs += @("-DPROJECT_ENABLE_TOOLS=ON")
}

if (($null -ne $CMakeOptions) -and ($CMakeOptions.Count -gt 0)) {
  $CMakeGeneratorArgs += $CMakeOptions
}

$CMakeGeneratorArgsOuput = '"' + ($CMakeGeneratorArgs -Join '" "') + '"'
Write-Output "[@$RunCMakeDir]: cmake .. $CMakeGeneratorArgsOuput"

Set-Location $ScriptDir

New-Item -Path "$BuildDirectory" -ItemType "directory" -Force

Set-Location $BuildDirectory

& cmake .. $CMakeGeneratorArgs
