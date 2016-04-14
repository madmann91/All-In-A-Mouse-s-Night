@echo off

del lights.pak
del world.map
..\export\tdmc -f lights.tdm -m 50
ren lights.map world.map
..\export\luac5.1 lights.lua
ren luac.out game.lua

rem Add files
..\export\packer -f lights.pak -a world.map -a game.lua -a info.txt
..\export\packer -f lights.pak -s

del game.lua
pause