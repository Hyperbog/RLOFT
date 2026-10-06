# CINE12.ANM

- Bytes: 1,446,666
- SHA-256: `a39bdb4965ba27f9fd1b1f4a8fb2ac291a48da831070cdf226faab229b132291`
- Source upload: `ANM+OTHER.zip`
- Family: ANM cinematic

Initial direct parsing distinguishes this file from `CINE00`-`CINE11`: it uses substantial chunk type-2/type-3 compressed image data and contains no type-8 fixed-size full-plane image chunks. Preserve that distinction during decoder reconstruction.
