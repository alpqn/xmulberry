# XMulberry
A GUI utility to set system settings for standalone window managers 

![tab1](https://github.com/user-attachments/assets/f247d353-8981-43f0-af8f-541868df764a)
![tab2](https://github.com/user-attachments/assets/3f69b6f3-9b2a-45ce-b9f2-0d986ccaa1b4)

## Usage
After the save, the settings are written to a text file in `$XDG_CONFIG_HOME/xmulberry/xmulberry`. The file can later be `source`d in `bashrc`, `xinitrc`, `zshrc` or wherever.
Xresources settings and MIME types won't be saved in the file; instead, they will be written to `Xresources` and `mimeapps.list` respectively. 

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
qmake6
make -j $(nproc)
```
