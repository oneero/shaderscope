@echo off

REM compile shaders

mkdir shader\build\boids

REM simple shader
third-party\build\bin\shaderc.exe ^
-f shader\boids\boids_vs.sc -o shader\build\boids\boids_vs.bin ^
--platform windows --type vertex --verbose -i ./ -p vs_5_0

third-party\build\bin\shaderc.exe ^
-f shader\boids\boids_fs.sc -o shader\build\boids\boids_fs.bin ^
--platform windows --type fragment --verbose -i ./ -p ps_5_0

third-party\build\bin\shaderc.exe ^
-f shader\boids\boids_cs_indirect.sc -o shader\build\boids\boids_cs_indirect.bin ^
--platform windows --type compute --verbose -i ./ -p cs_5_0

third-party\build\bin\shaderc.exe ^
-f shader\boids\boids_cs_init.sc -o shader\build\boids\boids_cs_init.bin ^
--platform windows --type compute --verbose -i ./ -p cs_5_0

third-party\build\bin\shaderc.exe ^
-f shader\boids\boids_cs_update.sc -o shader\build\boids\boids_cs_update.bin ^
--platform windows --type compute --verbose -i ./ -p cs_5_0