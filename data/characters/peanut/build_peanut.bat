@echo off

del peanut_backup.blend
copy peanut_anim.blend peanut_backup.blend
del peanut.mdl
del peanut.skt
..\..\export\tdmc -f peanut.tdm

pause
