## qrme

`qrme` is a quantum-resistant encrypted machine learning system designed to protect sensitive data and models against potential threats from [quantum computing](https://en.wikipedia.org/wiki/Quantum_computing). It utilises advanced cryptographic techniques to ensure the confidentiality and integrity of the machine learning model and its inputs/outputs, even in the face of future quantum attacks.


### Usage

You can start using `qrme` by executing:

```sh
make help
```

There's a minimal integration example [here](./create_sample_model.c). Don't forget to implement secure methods for key distribution and storage and ensure the integrity of the model file in a production environment.

### References

+ [Quantum-Resistant Cryptography](https://arxiv.org/abs/2112.00399)
+ [Applications of Post-quantum Cryptography](https://arxiv.org/abs/2406.13258)
+ [Preparing for Quantum-Safe Cryptography](https://www.ncsc.gov.uk/whitepaper/preparing-for-quantum-safe-cryptography)
+ [Next Steps in Preparing for Post-Quantum Cryptography](https://www.ncsc.gov.uk/whitepaper/next-steps-preparing-for-post-quantum-cryptography)
+ [The Impact of Quantum Computing on Present Cryptography](https://arxiv.org/abs/1804.00200)
+ [Post-Quantum Cryptography for Internet of Things: A Survey on Performance and Optimization](https://arxiv.org/abs/2401.17538)

### License

This project is licensed under the [BSD 3-Clause](LICENSE) License.

### Citation

```tex
@misc{qrme,
  author       = {Oketunji, A.F.},
  title        = {Quantum-Resistant Model Encryption (QRME)},
  year         = 2024,
  version      = {0.0.4},
  publisher    = {Zenodo},
  doi          = {10.5281/zenodo.13624471},
  url          = {https://doi.org/10.5281/zenodo.13624471}
}
```

### Copyright

(c) 2024 [Finbarrs Oketunji](https://finbarrs.eu). All Rights Reserved.
