$PSDefaultParameterValues['*:Encoding'] = 'UTF-8'

$OutputEncoding = [System.Text.UTF8Encoding]::new()

$ALL_CMAKE_FILES = Get-ChildItem -Depth 0 -File -Include "*.cmake.in", "*.cmake", "CMakeLists.txt"
$ALL_CMAKE_FILES += Get-ChildItem -Depth 0 -Directory -Exclude "build_jobs_*" | ForEach-Object -Process { Get-ChildItem -LiteralPath $_ -Recurse -File -Include "*.cmake.in", "*.cmake", "CMakeLists.txt" }

cmake-format -i $ALL_CMAKE_FILES
