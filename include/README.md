# include

This directory is a _copy_ of header files (files ending in `.h`) from
the [util](../util) and [GramsDataObj](../GramsDataObj) packages.

The presence of this directory makes easier to compose scripts based
on the examples in the [scripts](../scripts) directory. Without this
directory, the examples would have to refer to headers in, for
example, `../GramsSim/GramsDataObj/include`, which is longer to type
and assumes that the source directory and the build directory share a
common parent directory.

**IMPORTANT**: Making changes in this directory is a bad idea. Any
such changes will be over-written the next time you type a `cmake`
command. If you need to change a header file, change it in the [source
directory](../GramsSim) and issue the appropriate `cmake` command in
the build directory.

