#!/bin/bash

if [ "$1" == "" ] ; then
    echo "Usage: ./install-linux.sh <path to Shotcut.app>"
    exit 1
fi

cp -rp Release-Linux/bigsh0t-2.1.1-linux/lib/frei0r-1/* $1/lib/frei0r-1/
cp -rp Release-Linux/bigsh0t-2.1.1-linux/shotcut/share/shotcut/qml/filters/* $1/share/shotcut/qml/filters/
