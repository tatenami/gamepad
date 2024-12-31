g++ -shared -fPIC -O3 -o libpad.so padbase.cpp pad.cpp
g++ -shared -fPIC -O3 -o libpadros.so padbase.cpp

sudo mkdir /usr/local/include/pad
sudo cp pad*.hpp /usr/local/include/pad
sudo install libpad.so libpadros.so /usr/local/lib/
sudo ldconfig

rm ./lib*
