param(
    [string]$BuildDirectory = 'out/Release',
    [string]$Version = '0.1.0'
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$buildRoot = [IO.Path]::GetFullPath((Join-Path $projectRoot $BuildDirectory))
$packageRoot = Join-Path $projectRoot "releases/Chip8Emulator-$Version-windows-x64"
$zipPath = "$packageRoot.zip"
if (Test-Path -LiteralPath $packageRoot) { throw "Package directory already exists: $packageRoot" }
foreach ($required in @('Chip8Emulator.exe', 'shaders/vert.spv', 'shaders/frag.spv')) {
    if (!(Test-Path -LiteralPath (Join-Path $buildRoot $required))) { throw "Missing build file: $required" }
}
New-Item -ItemType Directory -Path "$packageRoot/shaders", "$packageRoot/licenses" -Force | Out-Null
Copy-Item -LiteralPath "$buildRoot/Chip8Emulator.exe" -Destination $packageRoot
Copy-Item -LiteralPath "$buildRoot/shaders/vert.spv", "$buildRoot/shaders/frag.spv" -Destination "$packageRoot/shaders"
Copy-Item -LiteralPath "$projectRoot/glm/copying.txt" -Destination "$packageRoot/licenses/GLM.txt"
Copy-Item -LiteralPath "$projectRoot/docs/MINIAUDIO_LICENSE.txt" -Destination "$packageRoot/licenses/miniaudio.txt"
Copy-Item -LiteralPath "$projectRoot/docs/THIRD_PARTY_NOTICES.md" -Destination "$packageRoot/licenses/THIRD_PARTY_NOTICES.md"
@'
CHIP-8 Emulator  Windows x64 preview

Requires Windows x64 and a graphics driver that supports Vulkan.
The Vulkan SDK and Visual Studio are not required to run this package.
The Microsoft C++ runtime is linked statically. No ROMs are included.

Extract the entire ZIP, keeping the shaders folder beside Chip8Emulator.exe.
Drag your own trusted .ch8 ROM onto Chip8Emulator.exe, or run in PowerShell:
  .\Chip8Emulator.exe 'C:\path\to\game.ch8'

PC keys 1 2 3 4 / Q W E R / A S D F / Z X C V map to the CHIP-8 keypad.
Release Escape to exit. Controls depend on the ROM.

This is a learning project with partial CHIP-8 compatibility. Audio works;
timing, interpreter quirks, and some malformed-ROM paths remain limited.
Only trusted ROMs should be used. The executable is not code-signed.

Source, build instructions, test scope and gameplay:
https://github.com/DVecc/Chip8Emulator
'@ | Set-Content -LiteralPath "$packageRoot/README.txt"
$unexpected = Get-ChildItem -LiteralPath $packageRoot -Recurse -File | Where-Object { $_.Extension -in @('.ch8', '.rom', '.pdb', '.obj') }
if ($unexpected) { throw 'Unexpected ROM or development files in package' }
Compress-Archive -Path "$packageRoot/*" -DestinationPath $zipPath
$hash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $([IO.Path]::GetFileName($zipPath))" | Set-Content -LiteralPath "$zipPath.sha256"
Write-Output $zipPath
Write-Output "SHA256: $hash"
