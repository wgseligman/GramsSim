# GRAMS Data Objects

A "data object" is a C++ type (class, struct, etc.) that is written to
or read from a ROOT file in GramsSim. 

The data objects are defined in [the include/
directory](./include). If any implementations are needed, they're
either in `.icc` files in the `include/` directory or in `.cc` files
in [the src/ directory](./src).

Because of the way [ROOT dictionary
generation](https://root.cern/manual/io_custom_classes/) works, the
single [Linkdef.hh](./include/LinkDef.hh) file must include the
necessary `#pragma` definitions for every data object. If you add a
new data object, be sure to edit this file.



