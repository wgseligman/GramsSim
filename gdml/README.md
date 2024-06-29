# GRAMS GDML files

_If you want a formatted (or easier-to-read) version of this file, scroll to the bottom of [`GramsSim/README.md`](../README.md) for instructions. If you're reading this on github, then it's already formatted._

This is an archive directory for various versions of the GRAMS
geometry as defined using the
[GDML](http://lcgapp.cern.ch/project/simu/framework/GDML/doc/GDMLmanual.pdf)
geometry language.

Typically the "main" [GRAMS GDML](../grams.gdml) file sits in the directory above this one. If you want to run [GramsG4](../GramsG4) on one of the files in this directory, either edit [`options.xml`](../options.xml) or use the `--gdmlfile=` option; e.g.,
```
./gramsg4 --gdmlfile=gdml/grams-z20.gdml
```

Consult the comments within each file to learn about the differences, or use the [diff](http://www.nevis.columbia.edu/cgi-bin/man.sh?man=diff) command; e.g.,
```
cd GramsSim/gdml
diff ../grams.gdml grams-z20.gdml
```
