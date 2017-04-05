# About

A new encryption algorithm designed under guidance from Dr. B. Dinesh Rao, professor at Manipal University.
This is a symmetric encryption algorithm and uses a variable key.

# Authors

Aurabindo J <mail@aurabindo.in>
Piyush Itankar <pitankar@gmail.com>

# Building

```
./configure
make
sudo make install
```

# Removing

Basic autotools targets like uninstall are available:

```
sudo make uninstall
```

# Usage

Since this a symmetric encryption algorithm, first create a
keybank by running the keygen program:
```
./keygen
```
It will generate `KEYBANK` and `XOR` files. Copy these to
the location where you want to encrypt/decrypt content.

```
cp ./KEYGEN ./XOR ../vaken
```
Now use the vaken cli to encrypt or decrypt
```
cd ../vaken
./vaken -e ../payload.txt
```
This will generate the encrypted file in the current directory.
You can decrypt it using:
```
./vaken -d ./encrypted
```
This will generate the decrypted output, which you can
compare with hexdump for accuracy. You will see some extra zeros.

# TODO

* Do cryptanalysis on the algorithm
* Remove the trailing zeros after decryption
