#!/bin/bash

export devtools=/home/lex/develop-tools
QTBASE=/home/lex/Qt5.7.0/5.7/gcc_64
export LD_LIBRARY_PATH=${QTBASE}/lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${devtools}/ffmpeg/ffmpeg_sources/ffmpeg-build/lib

export QTAVLIB_PATH=${PWD}/../lib_linux_x86_64
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${QTAVLIB_PATH}

#echo $LD_LIBRARY_PATH
#exit

echo "$( dirname "${SOURCE}" )"

#================================================================================
# Copyright (c) 2012 - 2013 by William Hallatt.
# 
# This script is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This script is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this script (GNUGPL.txt).  If not, see
#
#                    <http://www.gnu.org/licenses/>
# 
# Should you wish to contact me for whatever reason, please do so via:
#
#                 <http://www.goblincoding.com/contact>
# 
#================================================================================

# Copy your executable as well as this script to the directory where you 
# want to create the final tar ball.  To execute, simply pass the name of the
# executable as command line parameter to this script.
#
# Worth noting is that I built Qt from source myself, you may or may not need 
# additional plugins, etc and different or additional directory structures and
# will have to edit this script to suit your needs!

if [ $# -ne 1 ]
then
        echo "Usage: $0 <executable name>"
        exit 1
fi

executable=$1

# Obtain the Linux flavour and version.
distro=`lsb_release -d | awk '{print $2$3$4}' | sed 's/\./_/g'`

#date
datestr=`date +%Y%m%d%H%M`

# Create the directory that will be tarred up for distribution.
tardir=`echo $executable"_"$distro"_"$datestr | awk '{print tolower($0)}'`
mkdir $tardir
echo "Created tar ball directory: "$tardir

# Copy executable across.
chmod u+x $executable
cp $executable $tardir
echo "Copied executable "$executable" to "$tardir

# Create the libs directory.
libsdir=$PWD/$tardir/libs
mkdir $libsdir 
echo "Created libs directory: "$libsdir

# Copy all  dependencies across to the tar directory.
echo "Copying dependencies..."

for dep in `ldd ./$executable | awk '{print $3}' | grep -v "("`
do
  cp $dep $libsdir
  echo "Copied dependency "$dep" to "$libsdir
done


echo "Copying dependencies qtav...1"

for dep in `ldd ${QTAVLIB_PATH}/libQtAV.so | awk '{print $3}' | grep -v "("`
do
  cp $dep $libsdir
  echo "Copied dependency "$dep" to "$libsdir
done

echo "Copying dependencies qtav...2"

for dep in `ldd ${QTAVLIB_PATH}/libQtAVWidgets.so | awk '{print $3}' | grep -v "("`
do
  cp $dep $libsdir
  echo "Copied dependency "$dep" to "$libsdir
done




# Create the fonts directory and copy fonts across. You
# will obviously need to assign the directory path leading
# to your fonts to "fontdir", e.g. /home/you/qt/lib/fonts
qtfontsdir=${QTAVLIB_PATH}/fonts
fontsdir=$PWD/$tardir/fonts
mkdir $fontsdir
echo "Created fonts directory: "$fontsdir" copying fonts..."
cp -r $qtfontsdir/* $fontsdir

# You will need to change this to point to wherever libqxcb.so lives on your PC.

qtplugin=${QTBASE}/plugins
qtplatformplugin=${qtplugin}/platforms
qtplatformplugindir=$tardir/platforms
mkdir $qtplatformplugindir
echo "Created platforms directory: "$qtplatformplugindir
cp $qtplatformplugin/libqxcb.so $qtplatformplugindir
echo "Copied platform "$qtplatformplugin" to "$qtplatformplugindir

mkdir $qtplatformplugindir/xcbglintegrations
cp $qtplugin/xcbglintegrations/*.so $qtplatformplugindir/xcbglintegrations



# Create the script to fix xcb dependencies.
fixscript=$qtplatformplugindir/fixdep.sh
echo "Creating fixdep script: "$fixscript

echo "#!/bin/sh" >> $fixscript
echo "" >> $fixscript
echo "#================================================================================" >> $fixscript
echo "# Copyright (c) 2012 - 2013 by William Hallatt." >> $fixscript
echo "#" >> $fixscript
echo "# This script is free software: you can redistribute it and/or modify it under" >> $fixscript
echo "# the terms of the GNU General Public License as published by the Free Software" >> $fixscript
echo "# Foundation, either version 3 of the License, or (at your option) any later" >> $fixscript
echo "# version." >> $fixscript
echo "#" >> $fixscript
echo "# This script is distributed in the hope that it will be useful, but WITHOUT" >> $fixscript
echo "# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS" >> $fixscript
echo "# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details." >> $fixscript
echo "#" >> $fixscript
echo "# You should have received a copy of the GNU General Public License along with" >> $fixscript
echo "# this script (GNUGPL.txt).  If not, see" >> $fixscript
echo "#" >> $fixscript
echo "#                    <http://www.gnu.org/licenses/>" >> $fixscript
echo "#" >> $fixscript
echo "# Should you wish to contact me for whatever reason, please do so via:" >> $fixscript
echo "#" >> $fixscript
echo "#                 <http://www.goblincoding.com/contact>" >> $fixscript
echo "#" >> $fixscript
echo "#================================================================================" >> $fixscript
echo "" >> $fixscript
echo "# This script will install all \"not found\" dependencies for ALL libraries " >> $fixscript
echo "# that exist within the directory from where it is executed. " >> $fixscript
echo "" >> $fixscript
echo "# All of the packages thus installed can be removed through running the generated " >> $fixscript
echo "# \"removedep.sh\" script if you wish to undo these changes." >> $fixscript
echo "" >> $fixscript
echo "removescript=removedep.sh" >> $fixscript 
echo "> \$removescript" >> $fixscript
echo "echo \"#!/bin/sh\" >> \$removescript" >> $fixscript
echo "" >> $fixscript
echo "# For each library in this directory..." >> $fixscript
echo "for lib in \`ls -F | grep -v @ | grep *.so\`" >> $fixscript
echo "do" >> $fixscript
echo "  # Determine the dependencies, find only those that are not on the system, exclude all Qt5 libraries," >> $fixscript
echo "  # print the name of the library (not full path) and extract the package name from the .so name itself." >> $fixscript
echo "  lddquery=\`ldd \$lib | grep -i \"not found\" | grep -i -v qt5 | awk '{print \$1}' | sed 's/\\(^.*\\)\\.so.*\$/\\1/'\`" >> $fixscript
echo "  #echo \$lddquery" >> $fixscript
echo "" >> $fixscript
echo "  echo \"Installing dependencies for library: \"\$lib" >> $fixscript
echo "  #echo \$lib" >> $fixscript
echo "" >> $fixscript
echo "  for dep in \$lddquery" >> $fixscript
echo "  do" >> $fixscript
echo "    echo \"Found dependency: \"\$dep" >> $fixscript
echo "" >> $fixscript
echo "    # Query apt-cache for the relevant package, excluding dev and dbg versions." >> $fixscript
echo "    packagequery=\`apt-cache search \$dep | grep -v dev | grep -v dbg | awk '{print \$1}'\`" >> $fixscript
echo "    echo \"Querying package list for: \"$packagequery" >> $fixscript
echo "" >> $fixscript
echo "    sudo apt-get install \$packagequery" >> $fixscript
echo "    echo \"Installing: \"\$packagequery" >> $fixscript
echo "    echo \"sudo apt-get remove \$packagequery\" >> \$removescript" >> $fixscript 
echo "  done" >> $fixscript
echo "done" >> $fixscript
echo "" >> $fixscript
echo "chmod u+x \$removescript" >> $fixscript

chmod u+x $fixscript

echo Copy COMMON QT Plathform lib
cp ${QTBASE}/lib/libQt5DBus.so.?    $libsdir
cp ${QTBASE}/lib/libQt5XcbQpa.so.?  $libsdir


# Edit this script to add whatever other additional plugins your application
# requires.
#qtsqliteplugin=/home/william/qtsource/qt5/qtbase/plugins/sqldrivers/libqsqlite.so
#qtsqliteplugindir=$tardir/sqldrivers
#mkdir $qtsqliteplugindir
#echo "Created sql driver directory: "$qtsqliteplugindir
#cp $qtsqliteplugin $qtsqliteplugindir
#echo "Copied "$qtsqliteplugin" to "$qtsqliteplugindir

# Create the run script.
execscript=$tardir/"run$executable.sh"
echo "Created run script: "$execscript

echo "#!/bin/sh" > $execscript
#echo "export QT_STYLE_OVERRIDE=qtk" >> $execscript
echo "export LD_LIBRARY_PATH=\`pwd\`/libs" >> $execscript
echo "export QT_QPA_FONTDIR=\`pwd\`/fonts" >> $execscript
echo "export QT_QPA_PLATFORM_PLUGIN_PATH=\`pwd\`/platforms" >> $execscript
echo "./$executable" >> $execscript

# Make executable.
chmod u+x $execscript

# Create a README
echo "Creating README..."

readme=$tardir/README
echo "================================================================================" >> $readme
echo "Please launch $executable via" >> $readme
echo "" >> $readme
echo "                 $execscript" >> $readme
echo "" >> $readme
echo "If you run into any trouble regarding dependencies, all you need to do is to" >> $readme
echo "run " >> $readme
echo "                 $fixscript " >> $readme
echo "" >> $readme
echo "in order to automatically resolve dependencies on your behalf " >> $readme
echo "(note that you will need administrator privileges for this as this script will" >> $readme
echo "download the necessary libraries for your platform). " >> $readme
echo "" >> $readme
echo "Should you wish to contact me for whatever reason, please do so via:" >> $readme
echo "" >> $readme
echo "                 <http://www.goblincoding.com/contact>" >> $readme
echo "" >> $readme
echo "================================================================================" >> $readme

echo "Creating tarball..."
tar -zcvf $tardir".tar" $tardir

echo "Cleaning up..."
#rm -rf $tardir
echo "Done!"
