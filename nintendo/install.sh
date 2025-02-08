g++ -shared -fPIC -O3 -o libprocon.so ../pad.cpp ../padbase.cpp procon.cpp

sudo cp procon.hpp /usr/local/include/pad

sudo install libprocon.so /usr/local/lib
sudo ldconfig

rm ./libprocon.so
