# ASGS
An Alpine diskless overlay for your very own satellite ground station!

## Requirements
You will need the following things:
- A SD card, obviously. You shouldn't need more than 2GB.
- A Raspberry Pi, obviously. 2GBs of RAM are *fine* but more is always better. Also, it should be able to run `aarch64`. That means only RPi 3B+ and up will work.
- The [overlay](https://github.com/sdrgeek/asgs/raw/main/asgs.sdrgeek.local.apkovl.tar.gz), obviously.
- [Alpine Linux](https://alpinelinux.org/downloads/). Make sure you get the Raspberry Pi `aarch64` specific image and it's the one ending with `.tar.gz`, not `.img.gz`.

## Formatting
First, find your SD: `lsblk`
```
➜  ~ lsblk
NAME                  MAJ:MIN RM   SIZE RO TYPE  MOUNTPOINTS
sda                     8:0    1   7.4G  0 disk  
├─sda1                  8:1    1   256M  0 part  
└─sda2                  8:2    1   7.2G  0 part  
<snip>
```
In my case I have inserted the SD card into an USB adapter. If you have a SD card slot you may see something like `mmcblk0pX` where X denotes the partition number. For this guide we will assume that the SD card is at `/dev/sda`.

Alternatively, execute `sudo dmesg` right after inserting the SD card. For example:
```
➜  ~ sudo dmesg
<snip>
[ 6493.015685] usb-storage 1-4:1.0: USB Mass Storage device detected
[ 6493.016321] scsi host4: usb-storage 1-4:1.0
[ 6494.041392] scsi 4:0:0:0: Direct-Access     Mass     Storage Device   1.00 PQ: 0 ANSI: 0 CCS
[ 6494.041994] sd 4:0:0:0: Attached scsi generic sg0 type 0
[ 6494.416634] sd 4:0:0:0: [sda] 15601664 512-byte logical blocks: (7.99 GB/7.44 GiB)
[ 6494.417014] sd 4:0:0:0: [sda] Write Protect is off
[ 6494.417024] sd 4:0:0:0: [sda] Mode Sense: 03 00 00 00
[ 6494.417239] sd 4:0:0:0: [sda] No Caching mode page found
[ 6494.417248] sd 4:0:0:0: [sda] Assuming drive cache: write through
[ 6494.420866]  sda: sda1 sda2
```

Now that we are at it: make a backup of anything important!

Next up is formatting the SD card. We will do that using the `fdisk` tool. If you get a `command not found` you'll have to install it. It's `sudo apt install fdisk` for Debian-based systems.

```
➜  ~ sudo fdisk /dev/sda

Welcome to fdisk (util-linux 2.38.1).
Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.


Command (m for help):
```

Create a new DOS (MBR) partition table by typing `o` (**WARNING**: this will delete everything upon writing your changes. There's no confirmation.)
```
Command (m for help): o
Created a new DOS (MBR) disklabel with disk identifier 0xb3a7fb94.
```
Now create the boot partition:
```
Command (m for help): n
Partition type
   p   primary (0 primary, 0 extended, 4 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (1-4, default 1): <Enter>
First sector (2048-15601663, default 2048): <Enter>
Last sector, +/-sectors or +/-size{K,M,G,T,P} (2048-15601663, default 15601663): +256M (You could make it just +100M, which are 100MB, if you feel like living on the edge.)

Created a new partition 1 of type 'Linux' and of size 256 MiB.
```

You may see something like this:
```
Partition #1 contains a vfat signature.

Do you want to remove the signature? [Y]es/[N]o: y

The signature will be removed by a write command.
```
If you've made a backup there's nothing to worry about. You made a backup, right? Right?

Cool! Now create your "data" partition:
```
Command (m for help): n
Partition type
   p   primary (1 primary, 0 extended, 3 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (2-4, default 2): <Enter>
First sector (526336-15601663, default 526336): <Enter>
Last sector, +/-sectors or +/-size{K,M,G,T,P} (526336-15601663, default 15601663): <Enter> (Or specify the size. You shouldn't need more than +1G, which is equivalent to 1GB.)

Created a new partition 2 of type 'Linux' and of size 7.2 GiB.
Partition #2 contains a ext4 signature.

Do you want to remove the signature? [Y]es/[N]o: y

The signature will be removed by a write command.
```

Good. The worst part is over. Making the whole thing bootable is rather easy:
```
Command (m for help): a
Partition number (1,2, default 2): 1

The bootable flag on partition 1 is enabled now.
```
You should choose partition 1 here because this is your boot partition.

Almost there! So the Raspi can "see" the boot partition, it needs to be of a specific type. Remember that a partition and a filesystem are two completely different things. Setting a partition type has nothing to do with the filesystem.

You can change the type like so:
```
Command (m for help): t
Partition number (1,2, default 2): 1
Hex code or alias (type L to list all): c

Changed type of partition 'Linux' to 'W95 FAT32 (LBA)'.
```

Now, there's only one last thing to do:
```
Command (m for help): w
The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.

➜  ~ 
```
Writing! If you didn't make a backup until now, then you, my friend, fucked up.

That's it for `fdisk`! Now create the filesystems.
1. First must be FAT32. This is where Alpine boots from.
```
➜  ~ sudo mkfs.vfat /dev/sda1
mkfs.fat 4.2 (2021-01-31)
```
Easy as that.

2. You can choose any filesystem that Alpine supports here. I chose `ext4` because why not and because Alpine uses it by default. One thing to note is that `^has_journal`. This turns off `ext4`'s "journaling" feature. That means that never ever something get written to the SD card by Alpine and therefore extending its insulting lifespan.
```
➜  ~ sudo mkfs.ext4 -O ^has_journal /dev/sda2
mke2fs 1.47.0 (5-Feb-2023)
Creating filesystem with 1884416 4k blocks and 471424 inodes
Filesystem UUID: bd211573-c4e3-4d7f-a1f5-f874553d0c9f
Superblock backups stored on blocks: 
        32768, 98304, 163840, 229376, 294912, 819200, 884736, 1605632

Allocating group tables: done                            
Writing inode tables: done                            
Writing superblocks and filesystem accounting information: done
```

## Actually installing Alpine
I've just used my DEs archive tool but if you are keen on it you can do it like that on the console (not tested):
```
sudo mount /dev/sda1 /mnt
cd /mnt
sudo tar xzf /path/to/your/alpine.tar.gz
cd ~
sudo umount /mnt
```
If you're using your DEs archive tool too, make sure you don't create a folder. All files and folders inside the `.tar.gz` must be in the partition's root.

## Installing the overlay
```
sudo mount /dev/sda2 /mnt
cd /mnt
sudo cp ~/Downloads/asgs.sdrgeek.local.apkovl.tar.gz .
```

Now you'll need to create your wifi credentials file:
```
wpa_passphrase <SSID> | sudo tee wpa_supplicant.conf
```

Don't forget to unmount and you're GTG.
```
cd ~
sudo umount /mnt
```
