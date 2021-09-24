Write-Host "Make addons links ..."

$PSScriptRoot = Split-Path $MyInvocation.MyCommand.Path -Parent

$OFRootPath = (get-item $PSScriptRoot).Parent.Parent.Parent.FullName

Write-Host "from $PSScriptRoot"
Write-Host "to $OFRootPath"

Get-ChildItem "$PSScriptRoot\addons"| 
Foreach-Object {
    $fromPath = $_.FullName
    $toPath = "$OFRootPath\addons\$_"
    Write-Host "link $fromPath -> $toPath"
    Copy-Item -Path $fromPath -Destination $toPath -Recurse
}