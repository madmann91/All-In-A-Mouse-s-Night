@echo off
..\export\luac5.1 characters.lua
del characters.lib
ren luac.out characters.lib
..\export\luac5.1 weapons.lua
del weapons.lib
ren luac.out weapons.lib
pause