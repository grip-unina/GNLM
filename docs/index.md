---
layout: paper
paper: Guided patch-wise nonlocal SAR despeckling
github_url: https://github.com/grip-unina/GNLM
authors: 
  - name: Sergio Vitale
    link: https://grip-unina.github.io/GNLM/
    index: 1
     
  - name: Davide Cozzolino
    link: http://www.grip.unina.it/people/userprofile/davide_cozzolino.html
    index: 2
     
  - name: Giuseppe Scarpa
    link: http://www.grip.unina.it/people/userprofile/giuseppe_scarpa.html
    index: 2
     
  - name: Luisa Verdoliva
    link: http://www.grip.unina.it/people/userprofile/verdoliva.html
    index: 2
    
  - name: Giovanni Poggi
    link: http://www.grip.unina.it/people/userprofile/poggi.html
    index: 2
    
affiliations: 
  - name: Engineering Department of University Parthenope, Naples, Italy
    index: 1
  - name: Department of Electrical Engineering and Information Technology, University Federico II of Naples, Italy
    index: 2
    
links:
    arxiv: https://arxiv.org/abs/1811.11872
    code: https://github.com/grip-unina/GNLM
---


![header](./header.jpg)

We propose a new method for SAR image despeckling which leverages information drawn from co-registered optical imagery.
Filtering is performed by plain patch-wise nonlocal means, operating exclusively on SAR data.
However, the filtering weights are computed by taking into account also the optical guide, which is much cleaner than the SAR data,
and hence more discriminative. To avoid injecting optical-domain information into the filtered image, a SAR-domain statistical
test is preliminarily performed to reject right away any risky predictor.
Experiments on two SAR-optical datasets prove the proposed method to suppress very effectively the speckle, preserving 
structural details, and without introducing visible filtering artifacts. Overall, the proposed method compares favourably with
all state-of-the-art despeckling filters, and also with our own previous optical-guided filter.

### News

*   2018-11-29: Paper available on Arxiv.

### Bibtex

```js
@article{Guided_despeckling, 
author={S. Vitale and D. Cozzolino and G. Scarpa and L. Verdoliva and G. Poggi}, 
journal={arXiv:1811.11872}, 
title={Guided patch-wise nonlocal SAR despeckling}, 
year={2018} 
}
```
