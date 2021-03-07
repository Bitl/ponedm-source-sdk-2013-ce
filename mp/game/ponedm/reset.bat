@ECHO OFF
del voice_ban.dt
del serverconfig.vdf
del stats.txt
del ponedm_pak.vpk.sound.cache
del modelsounds.cache
del hl2mp_shared.vpk.sound.cache
del demoheader.tmp
del textwindow_temp.html
del cfg\config.cfg
del cfg\settings.scr
del cfg\user.scr
del bin\client.pdb
del bin\server.pdb
rmdir /S /Q maps\graphs\
rmdir /S /Q download\user_custom\
rmdir /S /Q sound\
rmdir /S /Q materials\
rmdir /S /Q models\
rmdir /S /Q download\sound\
rmdir /S /Q download\materials\
rmdir /S /Q download\models\
rmdir /S /Q downloadlists\
COPY "..\..\..\README.md" README-AND-CREDITS.txt
pause
exit