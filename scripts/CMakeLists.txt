# Install example Geant4 macro files
set(root_scripts
		SimpleAnalysis.C
		Hist2Text.C
		sillyEnergyFlux.root
		energyflux.txt
		)

foreach(_script ${root_scripts})
  configure_file(
    ${_script}
    ${_script}
    COPYONLY
    )
endforeach()
