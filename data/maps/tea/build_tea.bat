@echo off

del tea_backup.blend
copy tea.blend tea_backup.blend

del world.map
..\..\export\tdmc -f tea.tdm -m 50
ren tea.map world.map
..\..\export\luac5.1 tea.lua
del game.lua
ren luac.out game.lua

pause