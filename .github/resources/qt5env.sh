#!/bin/bash

QT_BASE_DIR=/opt/qt515

export QTDIR=$QT_BASE_DIR
export PATH=$QT_BASE_DIR/bin${PATH:+:}$PATH

if [[ $(uname -m) == "x86_64" ]]; then
  export LD_LIBRARY_PATH=$QT_BASE_DIR/lib/x86_64-linux-gnu:$QT_BASE_DIR/lib${LD_LIBRARY_PATH:+:}$LD_LIBRARY_PATH
else
  export LD_LIBRARY_PATH=$QT_BASE_DIR/lib/i386-linux-gnu:$QT_BASE_DIR/lib${LD_LIBRARY_PATH:+:}$LD_LIBRARY_PATH
fi

export PKG_CONFIG_PATH=$QT_BASE_DIR/lib/pkgconfig${PKG_CONFIG_PATH:+:}$PKG_CONFIG_PATH
