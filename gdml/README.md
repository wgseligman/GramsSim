# GRAMS GDML files

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
