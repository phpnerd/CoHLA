#!/bin/bash

NR_OF_CORES=4
_CMAKE_FLAGS=-Wno-dev

TMP_DIR=~/orti_installer
C_DIR=${PWD}
INSTALL_DIR=/opt

OPENRTI_VERSION=0.9.0
OPENRTI_LOG=$TMP_DIR/OpenRTI.log
OPENRTI_BUILD_DIR=${TMP_DIR}/OpenRTI-${OPENRTI_VERSION}/build

FMILIB_VERSION=2.0.3
FMILIB_LOG=$TMP_DIR/FMI-lib.log
FMILIB_BUILD_DIR=${TMP_DIR}/FMILibrary-${FMILIB_VERSION}/build
FMILIB_INSTALL_DIR=/usr/local

BULLET_VERSION=2.86.1
BULLET_LOG=$TMP_DIR/Bullet.log
BULLET_BUILD_DIR=${TMP_DIR}/bullet3-${BULLET_VERSION}/build

ORTILIBS_LOG=$TMP_DIR/OpenRTI-libs.log
ORTILIBS_BUILD_DIR=${TMP_DIR}/OpenRTI-libs/build
ORTILIBS_INSTALL_DIR=${INSTALL_DIR}/OpenRTI-libs

# Update system
echo "[System] Updating"
sudo apt-get -qy update
sudo apt-get -qy upgrade

#Install dependencies
echo "[System] Installing dependencies"
sudo apt-get -qy install build-essential cmake git libglm-dev libglfw3-dev libboost-system-dev libboost-thread-dev libxerces-c-dev openjdk-8-jdk python3 unzip
# Create and move into temp directory
if [ ! -d $TMP_DIR ]; then
  mkdir $TMP_DIR
fi
cd $TMP_DIR

# Install OpenRTI
echo "[OpenRTI] Downloading"
if [ ! -f OpenRTI.tar.bz2 ]; then
  wget -O OpenRTI.tar.bz2 https://sourceforge.net/projects/openrti/files/OpenRTI-${OPENRTI_VERSION}.tar.bz2 -Lq
fi
echo "[OpenRTI] Extracting"
tar xf OpenRTI.tar.bz2
if [ ! -d $OPENRTI_BUILD_DIR ]; then
  mkdir $OPENRTI_BUILD_DIR
fi
cd $OPENRTI_BUILD_DIR
echo "[OpenRTI] Building"
cmake -DOPENRTI_BUILD_SHARED=ON $_CMAKE_FLAGS ../ > $OPENRTI_LOG
make -j${NR_OF_CORES} >> $OPENRTI_LOG 2>&1
sudo make install >> $OPENRTI_LOG
echo "[OpenRTI] Installed"
cd $TMP_DIR

# Install FMI-libary
echo "[FMI-lib] Downloading"
if [ ! -f FMI-lib.zip ]; then
  wget -O "FMI-lib.zip" https://jmodelica.org/fmil/FMILibrary-2.0.3-src.zip -Lq
fi
echo "[FMI-lib] Extracting"
unzip -uq FMI-lib.zip
if [ ! -d $FMILIB_BUILD_DIR ]; then
  mkdir $FMILIB_BUILD_DIR
fi
cd $FMILIB_BUILD_DIR
echo "[FMI-lib] Building"
cmake -DFMILIB_INSTALL_PREFIX=${FMILIB_INSTALL_DIR} -DFMILIB_BUILD_TESTS=OFF -DFMILIB_GENERATE_DOXYGEN_DOC=OFF $_CMAKE_FLAGS ../ > $FMILIB_LOG
make -j${NR_OF_CORES} >> $FMILIB_LOG 2>&1
sudo make install >> $FMILIB_LOG
echo "[FMI-lib] Installed"
cd $TMP_DIR

# Install Bullet
echo "[Bullet] Downloading"
if [ ! -f bullet.tar.gz ]; then
  wget -O bullet.tar.gz https://github.com/bulletphysics/bullet3/archive/${BULLET_VERSION}.tar.gz -Lq
fi
echo "[Bullet] Extracting"
tar xf bullet.tar.gz
if [ ! -d $BULLET_BUILD_DIR ]; then
  mkdir $BULLET_BUILD_DIR
fi
cd $BULLET_BUILD_DIR
echo "[Bullet] Building"
cmake -DBUILD_SHARED_LIBS=ON -DINSTALL_LIBS=ON -DINSTALL_EXTRA_LIBS=ON -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_PYBULLET=OFF -DBUILD_CPU_DEMOS=OFF -DBUILD_BULLET2_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF $_CMAKE_FLAGS ../ > $BULLET_LOG
make -j${NR_OF_CORES} >> $BULLET_LOG 2>&1
sudo make install >> $BULLET_LOG
echo "[Bullet] Installed"
cd $TMP_DIR

# Install CodeSynthesis XSD
echo "[CS-XSD] Downloading"
if [ ! -f cs-xsd.deb ]; then
  wget -O cs-xsd.deb https://www.codesynthesis.com/download/xsd/4.0/linux-gnu/x86_64/xsd_4.0.0-1_amd64.deb -Lq
fi
echo "[CS-XSD] Installing"
sudo dpkg -i cs-xsd.deb
echo "[CS-XSD] Installed"

# Install OpenRTI-libs
if [ ! -d OpenRTI-libs ]; then
  if [ -f ../OpenRTI-libs.tar.gz ]; then
    echo "[ORTI-libs] Extracting"
    mkdir OpenRTI-libs
    tar xf ../OpenRTI-libs.tar.gz -C OpenRTI-libs
  else
    echo "[ORTI-libs] Downloading"
    git clone https://gitlab.science.ru.nl/tnagele/OpenRTI-libs.git
  fi
  cd OpenRTI-libs
else
  cd OpenRTI-libs
  git pull
fi
if [ ! -d $ORTILIBS_BUILD_DIR ]; then
  mkdir $ORTILIBS_BUILD_DIR
fi
cd $ORTILIBS_BUILD_DIR
echo "[ORTI-libs] Building"
cmake -DCMAKE_INSTALL_PREFIX=${ORTILIBS_INSTALL_DIR} -DBUILD_SHARED_LIBS=ON $_CMAKE_FLAGS ../ > $ORTILIBS_LOG
make -j${NR_OF_CORES} >> $ORTILIBS_LOG 2>&1
sudo make install >> $ORTILIBS_LOG
echo "[ORTI-libs] Installed"
cd $TMP_DIR

# Install Rotalumis
echo "[Rotalumis] Downloading"
if [ ! -f rotalumis ]; then
  wget http://www.es.ele.tue.nl/rotalumis/executables/integration/linux/64bit/rotalumis -Lq
fi
echo "[Rotalumis] Installing"
sudo cp rotalumis /usr/local/bin/rotalumis
sudo chmod +x /usr/local/bin/rotalumis
echo "[Rotalumis] Installed"

ldconfig

cd $C_DIR
echo "Done!"
#rm $TMP_DIR -r
