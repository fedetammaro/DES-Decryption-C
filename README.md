
# DES-Decryption
Mid-term assignment developed together with [Federico Palai](https://github.com/palai103), using the OpenMP C framework to decrypt passwords using the DES algorithm given a password dictionary.
The only argument required by this program is an integer,  number of runs for each password, to get a mean time of execution and avoid spikes of load on the CPU.

The requirements to run this program in CLion or by building with CMake are:
 - **crypt library** to use the `crypt`and `r_crypt`functions
 - **[OpenMP C framework](https://www.openmp.org/)** to handle the parallel sections inside the code
 
 Full project report can be found [here](https://github.com/Sfullez/DES-Decryption-C/blob/master/Project%20report.pdf)
