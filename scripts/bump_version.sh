#!/bin/sh
cd src;
cfile='version.c';
hfile='version.h';
echo "#include \"${hfile}\"" > $cfile; 
echo ''>> $cfile;
echo 'const char* getVersion(){' >> $cfile;
echo -n '    const char* VERSION="' >> $cfile;
echo "${1}\";" >> $cfile;
echo '    return VERSION;' >> $cfile; 
echo '}' >> $cfile;

