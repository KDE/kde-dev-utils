#! /bin/sh

export MALLOC_TREE=kminspector.tree
export MALLOC_THRESHOLD=2000
export LD_PRELOAD=${KDE_INSTALL_LIBDIR}/libktrace.so

$*

cat kminspector.tree | less
