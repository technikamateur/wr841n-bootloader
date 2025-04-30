#!/bin/bash

# Define colors
RED='\033[0;31m'       # Red for errors
YELLOW='\033[0;33m'    # Yellow for warnings
BLUE='\033[0;34m'      # Blue for informational messages
NC='\033[0m'           # No color

# Function to display usage
usage() {
  echo -e "${BLUE}Usage: $0 <flash_chip_size> <bootloader_image_filename> <openwrt_image_filename> <art_partition_filename> <mac_address> <router_version>${NC}"
  echo -e "${YELLOW}Router version must be one of: 09, 10, 11${NC}"
  exit 1
}

# Check if 6 arguments are provided
if [ "$#" -ne 6 ]; then
  echo -e "${RED}Error: Incorrect number of arguments provided.${NC}"
  usage
fi

# Assign arguments to variables
flash_chip_size="$1"
bootloader_image_filename="$2"
openwrt_image_filename="$3"
art_partition_filename="$4"
mac_address="$5"
router_version="$6"

# Display the input arguments
echo -e "${BLUE}Flash Chip Size:${NC} $flash_chip_size KiB"
echo -e "${BLUE}Bootloader Image Filename:${NC} $bootloader_image_filename"
echo -e "${BLUE}OpenWrt Image Filename:${NC} $openwrt_image_filename"
echo -e "${BLUE}ART Partition Filename:${NC} $art_partition_filename"
echo -e "${BLUE}MAC Address:${NC} $mac_address"
echo -e "${BLUE}Router Version:${NC} $router_version"

# Validate flash chip size (basic check for numeric value)
if ! [[ "$flash_chip_size" =~ ^[0-9]+$ ]]; then
  echo -e "${RED}Error: Flash chip size must be a numeric value.${NC}"
  exit 1
fi

# Validate MAC address (basic check)
if [[ ! "$mac_address" =~ ^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$ ]]; then
  echo -e "${RED}Error: Invalid MAC address format.${NC}"
  exit 1
fi

# Validate router version (must be 09, 10, or 11)
if [[ ! "$router_version" =~ ^(09|10|11)$ ]]; then
  echo -e "${RED}Error: Router version must be one of: 09, 10, 11.${NC}"
  exit 1
fi

# Create an empty output file of the given size, filled with 0xFF bytes
output_file="out.bin"
echo -e "${BLUE}Creating an empty file of size $flash_chip_size KiB filled with 0xFF...${NC}"
dd if=/dev/zero ibs=1k count="$flash_chip_size" | tr "\000" "\377" > "$output_file"
echo -e "${BLUE}File '$output_file' created successfully.${NC}"

# Validate and copy the bootloader image
if [ -f "$bootloader_image_filename" ]; then
  echo -e "${BLUE}Copying bootloader image '$bootloader_image_filename' into '$output_file'...${NC}"
  dd conv=notrunc if="$bootloader_image_filename" of="$output_file"
  echo -e "${BLUE}Bootloader image copied successfully.${NC}"
else
  echo -e "${RED}Error: Bootloader image file '$bootloader_image_filename' not found.${NC}"
  exit 1
fi

# Calculate the seek value for the ART partition
seek_value=$((flash_chip_size - 64)) # Subtract 64 KiB from the flash_chip_size
echo -e "${BLUE}Calculated seek value: $seek_value KiB (for writing ART partition)${NC}"

# Validate and copy the ART partition file
if [ -f "$art_partition_filename" ]; then
  echo -e "${BLUE}Copying ART partition file '$art_partition_filename' into '$output_file' at seek=$seek_value KiB...${NC}"
  dd conv=notrunc obs=1k seek="$seek_value" if="$art_partition_filename" of="$output_file"
  echo -e "${BLUE}ART partition file copied successfully.${NC}"
else
  echo -e "${RED}Error: ART partition file '$art_partition_filename' not found.${NC}"
  exit 1
fi

# Validate and copy the OpenWrt image into out.bin at seek=128 KiB
if [ -f "$openwrt_image_filename" ]; then
  echo -e "${BLUE}Copying OpenWrt image '$openwrt_image_filename' into '$output_file' at seek=128 KiB...${NC}"
  dd conv=notrunc obs=1k seek=128 if="$openwrt_image_filename" of="$output_file"
  echo -e "${BLUE}OpenWrt image copied successfully.${NC}"
else
  echo -e "${RED}Error: OpenWrt image file '$openwrt_image_filename' not found.${NC}"
  exit 1
fi

# Write the MAC address at the fixed offset (130048 bytes)
echo -e "${BLUE}Using fixed offset 130048 bytes for the MAC address.${NC}"

# Convert the MAC address into hex format for writing
mac_hex=$(echo "$mac_address" | sed 's/:/ /g' | xargs printf '\\x%s')

# Write the binary MAC address into the image
printf "$mac_hex" | dd conv=notrunc bs=1 seek=130048 count=6 of="$output_file"
echo -e "${BLUE}MAC address '$mac_address' written to '$output_file' at offset 130048 bytes.${NC}"

# Write the version string at the fixed offset (130304 bytes)
echo -e "${BLUE}Using fixed offset 130304 bytes for the version string.${NC}"
printf '\x08\x41\x00'"$(printf '\x%s' "$router_version")"'\x00\x00\x00\x01' | dd conv=notrunc bs=1 seek=130304 count=8 of="$output_file"
echo -e "${BLUE}Version string '$router_version' written to '$output_file' at offset 130304 bytes.${NC}"

# Add your custom processing logic here
echo -e "${BLUE}Script execution completed successfully.${NC}"

exit 0
