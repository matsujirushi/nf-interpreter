# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.

[CmdletBinding(SupportsShouldProcess = $true)]
param (
    [Parameter(Mandatory=$true)]
    [ValidateSet("ESP32", "STM32", "NXP", "TI", "RP2040")]
    [string]$TargetSeries,
	[Parameter(HelpMessage = "Enter the path to the folder where the tools should be installed.")][string]$Path,
    	[Parameter(HelpMessage = "Please enter the COM port for ESP32 flash utility [e.g. COM1].")][string]$COMPort,
    [switch]$force = $false
)

$commandArgs = ""

# any path set?
if (-not [string]::IsNullOrEmpty($Path)) {
    $commandArgs = " -Path '$Path'"
}

# need to pass the 'force' switch?
if ($force) {
    $commandArgs += " -force"
}

# need to pass the 'COMPort' parameter?
if ($COMPort) {
    $commandArgs += " -COMPort '$COMPort'"
}

if($TargetSeries -eq "ESP32")
{
    
    "****************************************************************************************" | Write-Host -ForegroundColor Yellow
    "Please download the ESP-IDF Windows Installer from https://dl.espressif.com/dl/esp-idf/"  | Write-Host -ForegroundColor Yellow
    "and install the various tools. If you're already done that, you can ignore this message." | Write-Host -ForegroundColor Yellow
    "****************************************************************************************" | Write-Host -ForegroundColor Yellow

    Invoke-Expression "$PSScriptRoot\install-esp32-tools.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "STM32") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "NXP") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
    Invoke-Expression "$PSScriptRoot\install-srecord.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "TI") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
}
elseif ($TargetSeries -eq "RP2040") {
    Invoke-Expression "$PSScriptRoot\install-stm32-tools.ps1 $commandArgs"
}
else {
    # shouldn't reach here
}
