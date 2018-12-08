# GNLM - Guided Non-Local Means
[Guided patch-wise nonlocal SAR despeckling](https://arxiv.org/abs/1811.11872) is 
a method for SAR image despeckling which leverages information drawn from co-registered optical imagery.

## Team members
 Sergio Vitale    (sergio.vitale@uniparthenope.it);
 Davide Cozzolino (davide.cozzolino@unina.it);
 Giuseppe Scarpa  (giscarpa@.unina.it);
 Luisa Verdoliva  (verdoliv@.unina.it);
 Giovanni Poggi   (poggi@.unina.it).
 
## License
Copyright (c) 2018 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
All rights reserved.
This software should be used, reproduced and modified only for informational and nonprofit purposes.

By downloading and/or using any of these files, you implicitly agree to all the
terms of the license, as specified in the document LICENSE.txt
(included in this package) 

## Prerequisits
All the functions and scripts were tested on LINUX 64bits and MATLAB 2017a,
the operation is not guaranteed with other configurations.
The software uses the OpenCV 2.1.0 library (http://opencv.org/), all
necessary files, for LINUX 64bits, are included in the repository. 

The command to compile: 

```
cd ./matlab
matlab -r compile_a64
```

## Usage
To execute, use the matlab script `guidedNLMeans.m` in `matlab` folder.
For help on how to use this script, you can e.g. use `help guidedNLMeans`.
You can find an examples in `demo_GNLM.m`.
