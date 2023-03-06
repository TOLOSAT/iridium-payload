# Data measurement
All those data were retrieved on the 21/03/22 by Cyril Cailhol from DEOS Department, Supaero, and Alexandre Laborie, Iridium subsystem leader. They were measured in Supaero's anechoic chamber, at the 2nd floor of building 007.  
  
# What is inside each folder
* **AxialRatio**: The axial ratio is a measure of the *polarization purity* of an antenna, which indicates how well the antenna is able to maintain a single polarization state when transmitting or receiving signals. The data in this folder is likely related to measurements of the axial ratio of the antenna, which may have been taken at different frequencies or under different conditions.

* **E-Left**, respectively **E-Right**: These folders contain data related to measurements of the electric field (E-field) of the antenna under test, taken with left, respectively right polarizations. 

* **SEQ_0 and SEQ_1**: ?   {Ask Cyril Cailhol for more info}

# Explanation of the naming convention
In each folder, you find two kind of files. The .amp describes the amplitude, the .pha describes the phase. The .pha and .amp file extensions are commonly used in the context of Fourier transform spectroscopy.   
Each file's name follows this scheme: iri_xxx_yyy.amp or iri_xxx_yyy.pha.  
The analysis we made is a 2-dimension one, thus we need two angles to describe it (theta and phi). The first one is theta, which is changing in a file (each new line is a new theta value). xxx describes the second angle, phi. In this measurement, its value could only be 0° (xxx = 00) or 90° (xxx = 001).  
yyy describe the frequency. The range is 1.610 to 1.630 Ghz, with a step size of 0.001 GHz. 
As a reminder, the mean frequency of Iridium is 1.621 GHz.  