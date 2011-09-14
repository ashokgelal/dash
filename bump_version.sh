#!/bin/bash
cd src;
file='version.c';
echo 'const char* version(){' > $file;
echo -n '    const char* VERSION="' >> $file;
echo "${1}\";" >> $file;
echo '    return VERSION;' >> $file; 
echo '}' >> $file;

