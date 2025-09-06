#!/bin/sh
count=$#
echo $count
total=0
for arg in "$@"; do
    total=$(echo "$total + $arg" | bc)
done

average=$(echo "scale=2; $total/$count" | bc)
echo $average
