@set D3DW_VERSION=0.2.0.1

@if not exist Tmp goto makedir
@del /S /F /Q Tmp
@rmdir Tmp
:makedir
@mkdir Tmp
@cd Tmp

@mt -nologo -inputresource:"../../Bin/D3DWd.dll";#2 -makecdfs -out:D3DWd.manifest || goto fail

@makecat D3DWd.manifest.cdf || goto fail

@signtool sign /f "../../D3DWorkshop/D3DW.pfx" /d "Direct3D Workshop" /du http://github.com/Lovesan/D3DWorkshop /t http://timestamp.verisign.com/scripts/timstamp.dll D3DWd.cat || goto fail

@mt -nologo -inputresource:"../../Bin/D3DW.dll";#2 -makecdfs -out:D3DW.manifest || goto fail

@makecat D3DW.manifest.cdf  || goto fail

@signtool sign /f "../../D3DWorkshop/D3DW.pfx" /d "Direct3D Workshop" /du http://github.com/Lovesan/D3DWorkshop /t http://timestamp.verisign.com/scripts/timstamp.dll D3DW.cat || goto fail

@"%WIX%\bin\candle" "../D3DWorkshop.wxs" -o D3DW.wixobj || goto fail

@"%WIX%\bin\light" D3DW.wixobj -ext WixUIExtension -o "D3DW-%D3DW_VERSION%.msi" || goto fail

@signtool sign /f "../../D3DWorkshop/D3DW.pfx" /d "Direct3D Workshop" /du http://github.com/Lovesan/D3DWorkshop /t http://timestamp.verisign.com/scripts/timstamp.dll "D3DW-%D3DW_VERSION%.msi" || goto fail

@move /Y "D3DW-%D3DW_VERSION%.msi" "../../Bin/" || goto fail

@goto ok

:fail
@echo Unable to make MSI installer
@exit 1

:ok
@echo MSI build succeeded
