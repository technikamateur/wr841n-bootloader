# wr841n-bootloader

This repository contains the source code extracted from the TP-Link WR841n v11 router's bootloader. It's created for educational and exploratory purposes.

## Prerequisites 🛠️

To avoid cluttering your main system, it's highly recommended to use a virtual machine or an LXC container. This project has been tested on a clean Debian 12 (bookworm) environment.

Run the following command to install the required packages:

```bash
sudo apt update && sudo apt install git make gcc g++ gcc-multilib lib32z1
```

## Building the Bootloader 🏗️
First, clone the repository to your local machine:
`git clone https://github.com/technikamateur/wr841n-bootloader.git`

### Setting Up the Toolchain 🔧

You'll need a MIPS cross-compiler. The OpenWRT GCC 4.3.3 toolchain is recommended. You can download it from the OpenWRT archive:
[OpenWRT GCC 4.3.3 Toolchain (ar71xx)](https://archive.openwrt.org/backfire/10.03.1/ar71xx/)

Note: After downloading, extract the toolchain to a suitable location on your system.

### Configuring the Build ⚙️
Modify the Build Configuration:
- Navigate to the `wr841n-bootloader/build` directory.
- Open the `Makefile` and update the `TOOLPATH` and `TOOLPREFIX` variables to reflect the location of your downloaded toolchain.

### Compile 
Simply run `make` inside the `wr841n-bootloader/build` directory.
