g++ -shared -fPIC -O3 -o libps5pad.so ../pad.cpp ../padbase.cpp ps5pad.cpp

sudo cp ps5pad.hpp /usr/local/include/pad

sudo install libps5pad.so /usr/local/lib
sudo ldconfig

rm ./libps5pad.so
