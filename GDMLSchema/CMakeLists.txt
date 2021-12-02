# Install GDML schema files

file(GLOB gdml_schema RELATIVE ${PROJECT_SOURCE_DIR}/GDMLSchema CONFIGURE_DEPENDS *.xsd)

foreach(_schema ${gdml_schema})
  configure_file(
    ${_schema}
    ${_schema}
    COPYONLY
    )
endforeach()
