#!/bin/bash

VERSION=2.6

if [ "$1" == "" ] ; then
    echo "Usage: ./install-linux.sh <path to Shotcut.app>"
    exit 1
fi

cp -rp Release-Linux/bigsh0t-$VERSION-linux/lib/frei0r-1/* $1/lib/frei0r-1/
cp -rp Release-Linux/bigsh0t-$VERSION-linux/shotcut/share/shotcut/qml/filters/* $1/share/shotcut/qml/filters/
