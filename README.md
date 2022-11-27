# Suboptimal Huffman Compression (SHC)

A messy implementation of Huffman coding to compress binary files.

## Compilation

Linux: Use provided Makefile to compile.

Windows: Edit Makefile to use a cross compiler or compile it directly on a Windows machine.

## Usage

Use -c flag to compress

Use - to compress from stdin

Use -d flag to decompress and add -stdout in place of output file name to output to stdout

Use -t flag to print the Huffman tree of compressed files
```
hsu@TREEHOUSE:~$ ls -l
total 1252
-rw-r--r-- 1 hsu hsu 1276235 Nov  8 06:31 example.txt
drwxr-xr-x 9 hsu hsu    4096 Nov  8 06:28 source
hsu@TREEHOUSE:~$ shc -c example.txt 
hsu@TREEHOUSE:~$ ls -l
total 1976
-rw-r--r-- 1 hsu hsu 1276235 Nov  8 06:31 example.txt
-rw-r--r-- 1 hsu hsu  739695 Nov  8 06:33 example.txt.shc
drwxr-xr-x 9 hsu hsu    4096 Nov  8 06:28 source
hsu@TREEHOUSE:~$ shc -d example.txt.shc example2.txt
hsu@TREEHOUSE:~$ md5sum example.txt
a1e490b38837162d2b83fea611c6c868  example.txt
hsu@TREEHOUSE:~$ md5sum example2.txt 
a1e490b38837162d2b83fea611c6c868  example2.txt
```

```
hsu@TREEHOUSE:~$ ls -l
total 12
-rw-r--r-- 1 hsu hsu  385 Nov 21 18:42 file0
-rw-r--r-- 1 hsu hsu  635 Nov 21 18:42 file1
drwxr-xr-x 9 hsu hsu 4096 Nov 21 13:11 source
hsu@TREEHOUSE:~$ tar -c file0 file1 --remove-files | shc - files.tar.shc
hsu@TREEHOUSE:~$ ls -l
total 8
-rw-r--r-- 1 hsu hsu 2982 Nov 21 18:43 files.tar.shc
drwxr-xr-x 9 hsu hsu 4096 Nov 21 13:11 source
hsu@TREEHOUSE:~$ shc -d files.tar.shc -stdout | tar -x
hsu@TREEHOUSE:~$ ls -l
total 16
-rw-r--r-- 1 hsu hsu  385 Nov 21 18:42 file0
-rw-r--r-- 1 hsu hsu  635 Nov 21 18:42 file1
-rw-r--r-- 1 hsu hsu 2982 Nov 21 18:43 files.tar.shc
drwxr-xr-x 9 hsu hsu 4096 Nov 21 13:11 source
```

```
hsu@TREEHOUSE:~$ shc -c text.txt
    Before compression: 448821
    After compression: 254373
    Compression ratio : 1.76
hsu@TREEHOUSE:~$ shc -t text.txt.shc
input the depth of the tree : 5

               +----40215<0>
               |    |
          +----77195<0>
          |    |
          |    +----36980<0>
          |         |
     +----148760<0>
     |    |
     |    +----71565( )
     |
+----265976<0>
|    |
|    |         +----32240<0>
|    |         |    |
|    |    +----62028<0>
|    |    |    |
|    |    |    +----29788(t)
|    |    |         |
```

## Details on file header

Magic numbers are `0x07 0x53 0x48 0x43` (^G S H C) which are followed by `0x00` or `0x80` depending on compile options.

After the initial 5 bytes follows the size of the uncompressed file in bytes stored over 8 bytes. 

After that is the frequency table used to generate Huffman tree to decompress the file. As it stands, in smaller files it increases the file size by a large amount.

At the end of the file exists the compressed data.
