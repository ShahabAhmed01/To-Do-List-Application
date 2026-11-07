# End-to-end smoke test for the TaskFlow console application.
# Usage: pwsh -File tests/smoke_test.ps1 -Binary .\taskflow.exe
param(
    [string]$Binary = ".\taskflow.exe"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $Binary)) {
    throw "Binary not found: $Binary"
}

function Invoke-Case {
    param(
        [string]$Name,
        [string[]]$InputLines,
        [string[]]$Expected
    )

    $scriptInput = (($InputLines -join "`n") + "`n")
    $output = $scriptInput | & $Binary 2>&1 | Out-String

    if ($LASTEXITCODE -ne 0) {
        throw "$Name exited with code $LASTEXITCODE`n$output"
    }

    foreach ($needle in $Expected) {
        if (-not $output.Contains($needle)) {
            throw "$Name did not contain: $needle`n$output"
        }
    }

    Write-Host "PASS: $Name"
}

Invoke-Case -Name "quit" -InputLines @("0") -Expected @(
    "TASKFLOW",
    "THANK YOU FOR USING TASKFLOW"
)

Invoke-Case -Name "full-flow" -InputLines @(
    "1", "Write smoke test", "",
    "2", "",
    "3", "", "1", "",
    "4", "", "2", "",
    "0"
) -Expected @(
    "Task added successfully",
    "Write smoke test",
    "Completed: 1",
    "Removed 1 completed task",
    "THANK YOU"
)

Invoke-Case -Name "invalid-input" -InputLines @("not-a-number", "", "0") -Expected @(
    "Invalid input",
    "THANK YOU"
)

Write-Host "All smoke tests passed."
