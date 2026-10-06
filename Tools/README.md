# Ravenloft Tools

This directory contains modern preservation and analysis utilities built from the certified reverse-engineering work in this repository.

## Current tools

- `anm/` — native inspector and frame extractor for `CINE00.ANM` through `CINE12.ANM`.

Tools must treat the shipped Ravenloft files as immutable source evidence. Decoders should follow the certified format documentation under `Resources/` and preserve format uncertainties rather than inventing semantics.
