#!/bin/bash

if [ -d ".git" ]; then
	cat ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut/alut.vcproj |sed 's/Version=\"8./Version=\"9./g' > ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut/alut900.vcproj
	cat ThirdParty/openal-soft-1.10.622/OpenAL_800.vcproj |sed 's/Version=\"8./Version=\"9./g' > ThirdParty/openal-soft-1.10.622/OpenAL_900.vcproj
	cat ThirdParty/ThirdPartyLib_800.vcproj |sed 's/Version=\"8./Version=\"9./g' > ThirdParty/ThirdPartyLib_900.vcproj

	cat Lepra/Lepra.vcproj |sed 's/Version=\"8./Version=\"9./g' > Lepra/Lepra900.vcproj
	cat TBC/TBC.vcproj |sed 's/Version=\"8./Version=\"9./g' > TBC/TBC900.vcproj
	cat Cure/Cure.vcproj |sed 's/Version=\"8./Version=\"9./g' > Cure/Cure900.vcproj

	cat UiLepra/UiLepra.vcproj |sed 's/Version=\"8./Version=\"9./g' > UiLepra/UiLepra900.vcproj
	cat UiCure/UiCure.vcproj |sed 's/Version=\"8./Version=\"9./g' > UiCure/UiCure900.vcproj
	cat UiTBC/UiTBC.vcproj |sed 's/Version=\"8./Version=\"9./g' > UiTBC/UiTBC900.vcproj

	cat Life/Life.vcproj |sed 's/Version=\"8./Version=\"9./g' > Life/Life900.vcproj
	cat Life/LifeClient/LifeClient.vcproj |sed 's/Version=\"8./Version=\"9./g' > Life/LifeClient/LifeClient900.vcproj
	cat Life/LifeServer/LifeServer.vcproj |sed 's/Version=\"8./Version=\"9./g' > Life/LifeServer/LifeServer900.vcproj

	cat UiCure/CureTestApp/CureTestApp.vcproj |sed 's/Version=\"8./Version=\"9./g' > UiCure/CureTestApp/CureTestApp900.vcproj

	cat UiCure/UiCure.sln |sed "s/[0-9]\{0,3\}\\.vcproj/900.vcproj/g" | sed "s/Format Version 9.00/Format Version 10.00/g" > UiCure/UiCure900.sln
else
	echo "Error: must be in git root dir!"
fi
