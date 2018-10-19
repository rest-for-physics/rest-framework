#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

# This script generates the version header to be used in REST installation
# J. Galan - Javier.Galan.Lacarra@cern.ch
# 8 - Oct - 2018

import os,sys,re,filecmp

outputHeader = sys.argv[1]
print " -- Generating TRestVersion.h"

branchName = os.popen( "git branch | grep -e \"^*\" | cut -d\' \' -f 2" ).read().rstrip("\n")

#print branchName

commit = os.popen('git rev-parse --verify HEAD' ).read().rstrip("\n")
#print commit
#print commit[0:8]

tag = os.popen( 'git describe --tags $(git rev-list --tags --max-count=1)' ).read().rstrip("\n")

#print tag

command = "git log -1 --format=%ai " + str( tag )
datetime = os.popen( command ).read().rstrip("\n")

date = datetime[0:10]
time = datetime[11:]

#print date
#print time

first =  tag.find(".")
last = tag.rfind(".")

a = int( re.sub("[^0-9]", "", tag[0:first] ) )
b = int( re.sub("[^0-9]", "", tag[first+1:last] ) )
c = int( re.sub("[^0-9]", "", tag[last+1:] ) )

restRelease = str(a) + "." + str(b) + "." + str(c)

code = a << 16 + b << 8 + c
codeA = a << 16 
codeB = b << 8
codeC = c 
code = codeA + codeB + codeC


f = open( "TRestVersion.tmp" , "w")

f.write("#ifndef REST_Version\n")
f.write("#define REST_Version\n")

f.write("/* Version information automatically generated by installer. */")

f.write("/*\n")
f.write(" * These macros can be used in the following way:\n")
f.write(" * \n" )
f.write(" * #if REST_VERSION_CODE >= REST_VERSION(2,23,4)\n")
f.write(" *     #include <newheader.h>\n" )
f.write(" * #else\n")
f.write(" *     #include <oldheader.h>\n" )
f.write(" * #endif\n")
f.write(" *\n" )
f.write(" */\n" )
f.write("#define REST_RELEASE \""+restRelease+"\"\n" )
f.write("#define REST_RELEASE_DATE \""+date+"\"\n" )
f.write("#define REST_RELEASE_TIME \""+time+"\"\n" )
f.write("#define REST_GIT_COMMIT \""+commit[0:8]+"\"\n" )
f.write("#define REST_GIT_BRANCH \""+branchName+"\"\n" )
f.write("#define REST_GIT_TAG \""+tag+"\"\n" )
f.write("#define REST_VERSION_CODE "+str(code)+"\n" )
f.write("#define REST_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))\n")

f.write("#endif\n")
f.close()

if filecmp.cmp( "TRestVersion.tmp", outputHeader ):
    os.remove( "TRestVersion.tmp" )
else:
    os.rename( "TRestVersion.tmp", outputHeader )
