#!/bin/bash

set -e

SDL2_PV=2.0.3

cachedir=.cache
mkdir -p $cachedir

echo `uname`

if [[ `uname` == "Darwin" ]]; then
    echo "Installation of OpenRCT2 assumes you have homebrew and use it to install packages."
    # Very possible I'm missing some dependencies here.
    brew install cmake wine

    if [[ ! -d /usr/include/wine ]]; then
        # This will almost certainly break as brew changes. Better ideas
        # welcome.
        sudo ln -s /usr/local/Cellar/wine/1.6.2/include/wine /usr/include
    fi

    mingw_dmg=gcc-4.8.0-qt-4.8.4-for-mingw32.dmg
    mingw_path=/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin
    if [[ ! -f $cachedir/$mingw_dmg ]]; then
        wget http://crossgcc.rts-software.org/download/gcc-4.8.0-qt-4.8.4-win32/$mingw_dmg --output-document $cachedir/$mingw_dmg
    fi

    if [[ ! -d $mingw_path ]]; then
        echo "Open the DMG file and install its contents"
        open $cachedir/$mingw_dmg
    fi

    echo "You will need to add $mingw_path to your \$PATH"
elif [[ `uname` == "Linux" ]]; then
    sudo apt-get install -y --force-yes binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686
fi

if [[ ! -f $cachedir/SDL2-devel-${SDL2_PV}-mingw.tar.gz ]]; then 
    wget http://libsdl.org/release/SDL2-devel-${SDL2_PV}-mingw.tar.gz --output-document $cachedir/SDL2-devel-${SDL2_PV}-mingw.tar.gz; 
fi
if [[ ! -d $cachedir/SDL2-${SDL2_PV} ]]; then
    pushd $cachedir 
        tar -xzf SDL2-devel-${SDL2_PV}-mingw.tar.gz 
    popd
fi

# Apply platform patch
mingw_patch=libsdl2-mingw-2.0.3-fix-platform-detection-for-mingw.patch
if [[ ! -f $cachedir/$mingw_patch ]]; then 
    wget "https://github.com/anyc/anyc-overlay/raw/master/media-libs/libsdl2-mingw/files/$mingw_patch" --output-document $cachedir/$mingw_patch; 

    # XXX not sure how to make this idempotent.
    pushd $cachedir/SDL2-${SDL2_PV}/i686-w64-mingw32/include/SDL2/ 
        echo "Applying patch."
        patch -p2 < ../../../../$mingw_patch 
    popd
fi

if [[ ! -d /usr/local/cross-tools ]]; then
    sudo mkdir -p /usr/local/cross-tools
fi
if [[ ! -d /usr/local/cross-tools/i686-w64-mingw32 ]]; then
    sudo cp -r $cachedir/SDL2-${SDL2_PV}/i686-w64-mingw32 /usr/local/cross-tools/
fi

if [[ ! -f $cachedir/i686-w64-mingw32-pkg-config ]]; then
    if [[ `uname` == "Darwin" ]]; then
        # BSD echo doesn't recognize the -e flag.
        echo "#! /bin/sh\nexport PKG_CONFIG_LIBDIR=/usr/local/cross-tools/i686-w64-mingw32/lib/pkgconfig\npkg-config \$@" > $cachedir/i686-w64-mingw32-pkg-config;
    else
        echo -e "#! /bin/sh\nexport PKG_CONFIG_LIBDIR=/usr/local/cross-tools/i686-w64-mingw32/lib/pkgconfig\npkg-config \$@" > $cachedir/i686-w64-mingw32-pkg-config;
    fi
fi

chmod +x $cachedir/i686-w64-mingw32-pkg-config
sudo cp $cachedir/i686-w64-mingw32-pkg-config /usr/local/bin/

ls -al /usr/local/bin | grep pkg-config
cat /usr/local/bin/i686-w64-mingw32-pkg-config