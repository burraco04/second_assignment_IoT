
$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$classes = Join-Path $root "build/classes"
$jssc = Join-Path $root "lib/jssc-2.9.4.jar"
$sources = Get-ChildItem -Path (Join-Path $root "src") -Recurse -Filter "*.java"

New-Item -ItemType Directory -Force -Path $classes | Out-Null
javac -encoding UTF-8 -cp $jssc -d $classes $sources.FullName
java -cp "$classes;$jssc" dru.DruApp
