# XMulberry
A GUI utility to set system settings for standalone window managers 
<img width="1920" height="1080"  src="https://github.com/user-attachments/assets/61ebd153-d9a5-452c-8a99-9935a4cd6e3b" />
<img width="1920" height="1080" src="https://github.com/user-attachments/assets/e061b3e2-323d-45a2-8824-5ceb74afd8f2" />


## Usage
After the save, the settings are written to a text file in `$XDG_CONFIG_HOME/xmulberry/xmulberry`. The file can later be `source`d in `bashrc`, `xinitrc`, `zshrc` or the equivalent rc file.
MIME types won't be saved in the file; instead, they will be written to `mimeapps.list`. 

## Dependencies
- Qt and qmake
- feh
- xrandr
- xset
- xinput

## Installation
### Arch:
```bash
sudo pacman -S qt6-base xorg-xrandr xorg-xset xorg-xinput feh
```
---
### Ubuntu/Debian:
```bash
sudo apt install qt6-base-dev x11-xserver-utils xinput feh
```
---
### Gentoo:
```bash
sudo emerge dev-qt/qt6-qtbase x11-apps/xrandr x11-apps/xset x11-apps/xinput app-graphics/feh
```
---
### Fedora:
```bash
sudo dnf install qt6-qtbase-devel xorg-x11-server-utils xorg-x11-utils feh  
```
## Compilation
```bash
mkdir build && cd build
cmake ..
cmake --build . --parallel
```
and run:
```bash
./xmulberry
```
