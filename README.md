# **myHRTFs**

## 1. Introduction myHRTFs.

<img width="433" alt="myHRTFs_top" src="https://user-images.githubusercontent.com/89256969/148159994-026fde6a-10b8-4eb2-9d18-6f0cbe71214f.png">

myHRTFs is real-time binaural audio object for Pure-data based on Head-Related Head Functions. It was built using the EigenHRTFs developed in our lab [1]. You can easily created binaural sound by using myHRTFs.

## 2. How to use myHRTFs

This section explains how to use myHRTFs.

### 2.1. Compile

First, please save all the files on GitHub in the same directory. Next, you must run the Makefile on terminal. so please type "make" to terminal and run it.

### 2.2. myhrtf~

If you compiled it using Makefile, you can use myHRTFs object in Pure-data. you need to write "myhrtf~" in object box when you use myHRTFs object. myhrtf~ has five inlets: inputting mono sound, azimuth, elevation, distance and pinna size (small:0, large:1). Azimuth can contain values between 0 and 359. When azimuth=360, the value will be automatically reverted to 0. Elevation can contain values between -40 and 90. Distance can contain values between 20 and 160. Pinna size is defined as small=0, large=1. myhrtf~ also has two outlet: binaural left sound and right sound. These will be output as values of type t_sample. 

<img width="898" alt="myhrtf~" src="https://user-images.githubusercontent.com/89256969/148172906-ad1f9735-ef99-4405-870f-915e7ba74214.png">

### 2.2. example of myhrtf~

I have created example.pd as an example using myhrtf~. First, please load your wav file. Next, press the binaural_sound_button to play the binaural sound source. If you want to adjust the position, you can use the slider. You can save the binaural sound source. First, please set new wav file to the open object and run it. Then you press start object and the recording begin. When you press stop object, the recording stop.

<img width="975" alt="examplepd" src="https://user-images.githubusercontent.com/89256969/148194313-055e640b-a06d-4355-bae3-cc8e71e6d321.png">

## 3. Lisence

Please see the Lisence file.

### References
[1] : C. Arévalo and J. Villegas, “Compressing head-related transfer function databases by eigen decom-position,” in2020 IEEE 22nd International Work-shop on Multimedia Signal Processing (MMSP).IEEE, 2020, pp. 1–6.