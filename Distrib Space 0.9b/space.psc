Name SPACE
Text This will install Space plugin for winamp on your computer
OutFile Space09.exe
DefaultDir $PROGRAMFILES\winamp
SetOutPath $PROGRAMFILES\winamp\skins
AddFile space\space_skin.zip
SetOutPath $VISDIR
AddFile vis_space.dll
SetOutPath $VISDIR\space\
AddFile space\vis_space.txt
AddFile space\space.bmp
AddFile space\space.theme
AddFile space\Aurore.spc
AddFile space\Bass Trip.spc
AddFile space\Cercles Dance.spc
AddFile space\Cercles Waves.spc
AddFile space\Colored Shower.spc
AddFile space\Cool Shape.spc
AddFile space\default.spc
AddFile space\Disco.spc
AddFile space\Distor Dream.spc
AddFile space\Ecstasy.spc
AddFile space\Fast Graph.spc
AddFile space\Fiesta.spc
AddFile space\Flashis.spc
AddFile space\Kalei Trip.spc
AddFile space\Luciols.spc
AddFile space\Moving Cyclones.spc
AddFile space\Nebula.spc
AddFile space\New Dimension.spc
AddFile space\Psychotic.spc
AddFile space\Radar.spc
AddFile space\RainBow Trip.spc
AddFile space\RainBow Trip Bis.spc
AddFile space\Random.spc
AddFile space\Snow Wind.spc
AddFile space\StarField.spc
AddFile space\StarField Bis.spc
AddFile space\Sunny.spc
AddFile space\Tunnel.spc
AddFile space\Vibration.spc
AddFile space\Vision Life.spc
AddFile space\Wet.spc
AddFile space\Wind.spc
AddFile space\Worm.spc
AddFile space\ZikBox.spc
ExecFile "$WINDIR\notepad.exe" $VISDIR\space\vis_space.txt
; Silent
