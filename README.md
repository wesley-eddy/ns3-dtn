This repository holds code and documentation on ns-3 module contributions to
support Delay/Disruption Tolerant Networking (DTN) Bundle Protocol (BP)
simulations.

## Usage

The "bp" subdirectory is intended to be added to an ns-3 distribution as a
module within either the "src" or "contrib" portions of the ns-3 source tree.

The "doc" subdirectory contains documents describing the organization of the bp
module source code.

The code is currently based on older ns-3 releases that used the waf build
system rather than the use of CMake in the newer ns-3 releases.

It has been used with ns-3.30.1.

## History

There is a 2013 Google Summer of Code (GSOC) project by Dizhi Zhou that we
experimented with as an initial basis for DTN simulations with BP routing
[SOCIS2013BundleProtocolProject](https://www.nsnam.org/wiki/SOCIS2013BundleProtocolProject).

Some aspects of that work are retained in this bp module code, however, there
were major changes and additions include:

* Bundle storage
* Refactoring of bundle blocks
* BPv7 bundle encoding/decoding support and CBOR
* Admin records
* Aggregate Custody Signalling
* UDP CLA
* Flowstats support for BP flows

The original names and copyright University of New Brunswick have been retained
in original files that were part of the 2013 GSOC project, even when those are
substantially altered.

## Contributors

Core team:

* Wesley Eddy
* Shawn McGraw
* Tim Walker

Several interns made major contributions during their time supporting the work:

* Reema Amhaz
* Pierce Forte
* Carter Edmond
* Bruce Barbour


