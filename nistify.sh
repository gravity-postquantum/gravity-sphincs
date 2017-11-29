#!/bin/sh

# generates the NIST submission package as a ZIP archive

DIR=`pwd`
cd $DIR/Additional_Implementations/fast && make clean
cd $DIR/Additional_Implementations/debug && make clean
cd $DIR/Reference_Implementation && make clean
cd $DIR/Supporting_Documentation/latex_source && make clean
cd $DIR/KAT && make clean
cd $DIR
cd ..
zip -r9 gravity-sphincs.zip gravity-sphincs --exclude "**.git*" "**.DS_Store*" "**nistify.sh" "**.vscode"
