# wr841n-bootloader
This repo contains the source code from TP-Link WR841n v11 router. I just crated this project for exploration.

## How to compile
I tested everything with a fresh Debian 12 bookworm. Steps to reproduce:
1. `apt install git make gcc g++ gcc-multilib lib32z1`
2. `git clone https://github.com/technikamateur/wr841n-bootloader.git`
3. `cd wr841n-bootloader/build`
4. `make fs_prep && make uboot`
