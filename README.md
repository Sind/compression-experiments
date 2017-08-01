Compression Experiments
=======================

This Repo contains the code for experiments I ran for my Master's thesis, "Energy Efficiency in Wireless Sensor Networks, Through Data Compression".

> Version: 1.0.0  

Compression test
----------------

The code for the first experiment can be found in the `compression_test` folder. It is a C/C++ code that measures the compression ratio of the compression algorithms, given different input sizes, and prints out a latex file with tables.


Energy costs of compression and transmission
--------------------------------------------

The code for the second experiment can be found in the `v4_workspace` folder. This is a [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio) workspace. To use it, open Simplicity Studio, and select `file > switch workspace`. It is C/C++ code that runs compression algorithms and data transmissions on the Flex Gecko microcontrollers, made to analyze the effect compression of sensor data has on the energy consumption of microcontrollers in a wireless sensor network. 


CREDITS
=======

The following compression algorithm implementations were used for my experiments:

 * [Snappy](https://github.com/google/snappy), by Google
 * [fpaq0](mattmahoney.net/dc/), by Matt Mahoney
 * [PPMs](www.compression.ru/ds/), by Dmitry Shkarin
 * [LZ4](http://lz4.github.io/lz4/), by Yann Collet
 * [LZW and LZW_MC](https://github.com/Sind/LZW), by Srod Karim

Data provided by NCAR/EOL under sponsorship of the National Science Foundation.

 * [HLY-07-01 SCS One Minute Data](https://data.eol.ucar.edu/dataset/102.012)
