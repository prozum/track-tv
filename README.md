track-tv
========
Tracks faces to adjust screen orientation

# Install Python Version
```bash
# Use pip to install pyFirmata 
sudo pip install pyfirmata

# Get source form github and enter source dir
git clone https://github.com/prozum/track-tv.git && cd track-tv

# Run track-tv
python track-tv.py
```

# Install C++ Version
```bash
# Get source form github and enter source dir
git clone https://github.com/prozum/track-tv.git && cd track-tv

# Generate ninja files with meson
mkdir build && meson build

# Compile
ninja -C build

# Run track-tv with address of cascade file as first parameter
./build/track-tv <cascade-file>
```
