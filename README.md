track-tv
========
Program to control screen tracking and following faces

# Installer Python Version
```bash
# Use pip to install pyFirmata 
sudo pip install pyfirmata

# Get source form github and enter source dir
git clone https://github.com/prozum/track-tv.git && cd track-tv

# Run track-tv
python track-tv.py
```

# Installer C++ Version
```bash
# Use pip to install pyFirmata 
sudo pip install pyfirmata

# Get source form github and enter source dir
git clone https://github.com/prozum/track-tv.git && cd track-tv

# Make build dir and enter build dir
mkdir build && cd build

# Generate Makefile with CMake
cmake ..

# Compile
make

# Run track-tv with adress of cascade file as first parameter
./track-tv <cascade-file>
```
