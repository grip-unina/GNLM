# ABSTRACT
We propose a new method for SAR image despeckling which leverages information drawn from co-registered optical imagery.
Filtering is performed by plain patch-wise nonlocal means, operating exclusively on SAR data.
However, the filtering weights are computed by taking into account also the optical guide, which is much cleaner than the SAR data,
and hence more discriminative. To avoid injecting optical-domain information into the filtered image, a SAR-domain statistical
test is preliminarily performed to reject right away any risky predictor.
Experiments on two SAR-optical datasets prove the proposed method to suppress very effectively the speckle, preserving 
structural details, and without introducing visible filtering artifacts. Overall, the proposed method compares favourably with
all state-of-the-art despeckling filters, and also with our own previous optical-guided filter.

