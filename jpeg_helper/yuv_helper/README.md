##### RGB => YCbCr

```
Y = a * R + b * G + c * B
Cb = (B - Y) / d
Cr = (R - Y) / e
```

##### YCbCr => RGB

```
R = Y + e * Cr
G = Y - (a * e / b) * Cr - (c * d / b) * Cb
B = Y + d * Cb
```

##### Coefficients

| | BT.601/JPEG | BT.709 | BT.2020 |
|-|-------------|--------|---------|
|a| 0.299       | 0.2126 | 0.2627  |
|b| 0.587       | 0.7152 | 0.6780  |
|c| 0.114       | 0.0722 | 0.0593  |
|d| 1.772       | 1.8556 | 1.8814  |
|e| 1.402       | 1.5748 | 1.4746  |

*d = (1 - c) / 0.5*\
*e = (1 - a) / 0.5*

##### Quantization

*(not applicable for JPEG)*

$D_Y' = INT[(219E_Y' + 16) \cdotp 2^{n-8}]$

$D_{CB}' = INT[(224E_{CB}' + 128) \cdotp 2^{n-8}]$

$D_{CR}' = INT[(224E_{CR}' + 128) \cdotp 2^{n-8}]$
