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
rmdir /S /Q maps\graphs\
rmdir /S /Q user_custom\
rmdir /S /Q sound\
rmdir /S /Q materials\
rmdir /S /Q models\
rmdir /S /Q downloadlists\
pause
exit