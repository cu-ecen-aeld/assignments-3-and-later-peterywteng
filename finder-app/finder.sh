#!/bin/sh
# Tester script for assignment 1

# Accepts the following runtime arguments: 
# the first argument is a path to a directory on the filesystem, 
# referred to below as filesdir; 
# the second argument is a text string which will be searched 
# within these files, referred to below as searchstr

# Requirement (1): Exits with return value 1 error and print statements if 
# any of the parameters were not specified
# $# should be 2 (3 parameters)
if [ $# -ne 2 ]; then
    echo "Please provide correct command options: finder.sh FILESDIR SEARCHSTR"
    exit 1
fi



# Requirement (2): Exits with return value 1 error and print statements if
# filesdir does not represent a directory on the filesystem
#if [ -d "/home/peter/cu-ecen-aeld/aesd-assignments/finder-app/conf" ]; then
#if [ -d $1 ]; then
    #echo "Yes, $1 is a directory!"
#else
    #echo "No, $1 is not a directory!"
    #echo "$1 does not represent a directory on the filesystem"
    #exit 1
#fi
if [ ! -d $1 ]; then
    echo "$1 does not represent a directory on the filesystem"
    exit 1
fi



# Requirement (3): Prints a message "The number of files are X and 
# the number of matching lines are Y" 
# where X is the number of files in the directory and all subdirectories
#   and Y is the number of matching lines found in respective files
#echo "Below is the file search result of $2 in $1:"
#find $1 -name $2

#echo "The number of files are:"
#find $1 -name $2 | wc -l
#NUMFILES=$(find $1 -name $2 | wc -l)
# the result will include the directory itself if without "/*" 
NUMFILES=$(find $1/* | wc -l)

#echo "Below is the string search result of $2 in $1:"
#grep -r $2 $1

#echo "The number of matching lines are:"
#grep -r $2 $1 | wc -l
NUMLINES=$(grep -r $2 $1 | wc -l)

echo "The number of files are ${NUMFILES} and the number of matching lines are ${NUMLINES}"



