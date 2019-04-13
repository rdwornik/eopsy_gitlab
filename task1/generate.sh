#!/bin/bash
set -e
set -u

#Skrypt do generowania folderów

for testdir in "$@"; do
    echo "create directory '$testdir'"
    mkdir -p $testdir
    for i in {1..4}; do
        counter=$(printf %02d $i)
        testfile=$testdir/${testdir##*/}$counter.txt
        echo "create file '$testfile'"
        echo -e "
        niemcy
        ukraina
        niemcy
        niemcy
        litwa
        łotwa 
        " > $testfile
    done
done

for testdir in "$@"; do
    echo "create directory '$testdir'"
    mkdir -p $testdir/$testdir
    for i in {1..4}; do
        counter=$(printf %02d $i)
        testfile=$testdir/$testdir/${testdir##*/}$counter.txt
        echo "create file '$testfile'"
        echo -e "
        niemcy
        ukraina
        niemcy
        niemcy
        litwa
        łotwa 
        " > $testfile
    done
done

for testdir in "$@"; do
    echo "create directory '$testdir'"
    mkdir -p $testdir/$testdir/$testdir
    for i in {1..4}; do
        counter=$(printf %02d $i)
        testfile=$testdir/$testdir/$testdir/${testdir##*/}$counter.txt
        echo "create file '$testfile'"
        echo -e "
        niemcy
        ukraina
        niemcy
        niemcy
        litwa
        łotwa  
        " > $testfile
    done
done



