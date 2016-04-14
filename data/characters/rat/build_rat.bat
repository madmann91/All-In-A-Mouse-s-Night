@echo off

del rat_backup.blend
copy rat_anim.blend rat_backup.blend
del rat.mdl
del rat.skt
..\..\export\tdmc -f rat.tdm

pause
