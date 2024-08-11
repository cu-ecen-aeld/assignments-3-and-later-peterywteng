#!/bin/sh
# Tester script for assignment 1

# Accepts the following arguments: 
# the first argument is a full path to a file (including filename) 
# on the filesystem, referred to below as writefile; 
# the second argument is a text string which will be written within this file,
# referred to below as writestr



# Requirement (1): Exits with return value 1 error and print statements if 
# any of the parameters were not specified

# $# should be 2 (3 parameters)
if [ $# -ne 2 ]; then
    echo "Please provide correct command options: writer.sh WRITEFILE WRITESTR"
    exit 1
fi



# Requirement (2): Creates a new file with name and path writefile 
# with content writestr, overwriting any existing file and 
# creating the path if it doesn’t exist.

mkdir -p "$(dirname "$1")" && touch "$1"

# Requirement (3): Exits with value 1 and error print statement if 
# the file could not be created.
if [ $? -ne 0 ]; then
    echo "$1 could not be created!"
    exit 1
fi

echo $2 > $1


