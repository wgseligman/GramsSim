# Install GDML schema files
set(gdml_schema 
		gdml_core.xsd
		gdml_define.xsd
		gdml_extensions.xsd
		gdml_materials.xsd
		gdml_parameterised.xsd
		gdml_replicas.xsd
		gdml_solids.xsd
		gdml.xsd
)

foreach(_schema ${gdml_schema})
  configure_file(
    ${_schema}
    ${_schema}
    COPYONLY
    )
endforeach()
