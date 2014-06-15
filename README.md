track-tv
========

# Installer Python Version
```bash
$ # Brug pip til at installere pyFirmata (Eller hent det direkte fra github)
$ sudo pip install pyfirmata

$ # Brug git til at hente kildekoden
$ git clone https://github.com/prozum/track-tv.git

$ # Gå ind i track-tv mappen
$ cd track-tv

$ # Kør programmet
$ python track-tv.py
```

# Installer C++ Version
```bash
$ # Brug git til at hente kildekoden
$ git clone https://github.com/prozum/track-tv.git

$ # Gå ind i track-tv mappen
$ cd track-tv

$ # Lav en byggemappe og gå ind i den
$ mkdir build && cd build

$ # Genere Makefile med CMake
$ cmake ..

$ # Kompilere program med make
$ make -j <antal kerner>

$ # Kør programmet med kaskade fil som første parameter
$ ./track-tv <kaskade fil>
```
