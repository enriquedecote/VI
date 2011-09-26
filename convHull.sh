#!/bin/bash

./main -a T -o u -w 0 -g 0.95 -e G
mv ../results/VI/debug.log ../results/VI/G.log

./main -a T -o u -w 0 -g 0.95 -e P
mv ../results/VI/debug.log ../results/VI/P.log

./main -a T -o u -w 0 -g 0.95 -e X
mv ../results/VI/debug.log ../results/VI/X.log

./main -a T -o u -w 0 -g 0.95 -e A
mv ../results/VI/debug.log ../results/VI/A.log

./main -a T -o u -w 0 -g 0.95 -e T
mv ../results/VI/debug.log ../results/VI/T.log

echo
exit 0
