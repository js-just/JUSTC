mkdir build
cd build
cmake ..
make

FILE=test/linux/test.justc

./justc $FILE

RESULT=$(cat $FILE)
ECHO $RESULT