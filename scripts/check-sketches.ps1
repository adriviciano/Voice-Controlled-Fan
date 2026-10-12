param([string]$ArduinoCli)
$ErrorActionPreference = 'Stop'
if (!$ArduinoCli) {
    $command = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if ($command) { $ArduinoCli = $command.Source }
    else { $ArduinoCli = 'C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe' }
}
if (!(Test-Path -LiteralPath $ArduinoCli)) { throw 'Specify -ArduinoCli with the Arduino CLI executable path.' }
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$targets = @(
    @{ Sketch='control-voz'; Board='esp32:esp32:esp32' },
    @{ Sketch='monitor-voz'; Board='esp32:esp32:esp32' },
    @{ Sketch='prueba-emisor'; Board='esp32:esp32:esp32' },
    @{ Sketch='prueba-receptor'; Board='esp32:esp32:esp32c3:CDCOnBoot=cdc' }
)
foreach ($target in $targets) {
    $build = Join-Path ([IO.Path]::GetTempPath()) ('vfan-' + $target.Sketch)
    Write-Output "Compiling $($target.Sketch) for $($target.Board)"
    & $ArduinoCli compile --fqbn $target.Board --build-path $build (Join-Path $root $target.Sketch)
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $($target.Sketch)" }
}
Write-Output 'All four sketches compiled. No firmware was uploaded.'
