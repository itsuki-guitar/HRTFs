# **HRTFs**

## 1. Introduction HRTFs.

<img width="500" alt="HRTFs_logo" src="https://user-images.githubusercontent.com/89256969/150991241-0c0edf5b-d740-4426-b1a5-aaae6a681997.png">

HRTFs is real-time spatializer object for Pure-data based on Head-Related Head Functions. It was built using the EigenHRTFs developed in our lab [1]. You can easily created binaural sound from a mono sound source by using HRTFs.

## 2. How to use HRTFs

This section explains how to use HRTFs.

### 2.1. Compile

First, please save all files on GitHub in the same directory. Next, please put m_pd.h in the same directory. Finally, you must run the Makefile on terminal, so please type "make" to terminal and run it.

### 2.2. hrtf~

If you compiled it using Makefile, you can use HRTFs object in Pure-data. you need to write "hrtf~" in object box when you use myHRTFs object. myhrtf~ has five inlets: inputting mono sound, azimuth, elevation, distance and pinna size (small:0, large:1). Azimuth can contain values between 0 and 359. When azimuth=360, the value will be automatically reverted to 0. Elevation can contain values between -40 and 90. Distance can contain values between 20 and 160. Pinna size is defined as small=0, large=1. myhrtf~ also has two outlet: binaural left sound and right sound. These will be output as values of type t_sample. 


<img width="500" alt="hrtf_simple" src="https://user-images.githubusercontent.com/89256969/150991710-def8d6ca-dda2-4840-8219-d2002343f19f.png">


### 2.2. Help file for hrtf~ 
You can experience hrtf~ in the help file. This help file uses [noise~] as a mono sound.  You can change the position of a binaural sound using three sliders and the size of a pinna using toggle.
<img width="500" alt="hrtf_help" src="https://user-images.githubusercontent.com/89256969/150994558-2a9f00c2-001a-4bef-96b8-66f171c2b3c2.png">

## 3. Lisence

Please see the Lisence file.

### References
[1] : C. Arévalo and J. Villegas, “Compressing head-related transfer function databases by eigen decom-position,” in2020 IEEE 22nd International Work-shop on Multimedia Signal Processing (MMSP).IEEE, 2020, pp. 1–6.