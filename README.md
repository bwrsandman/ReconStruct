[![Build Status](https://travis-ci.org/bwrsandman/ReconStruct.png?branch=python)](https://travis-ci.org/bwrsandman/ReconStruct)
[![Coverage Status](https://coveralls.io/repos/bwrsandman/ReconStruct/badge.png?branch=python)](https://coveralls.io/r/bwrsandman/ReconStruct?branch=python)
[![Dependency Status](https://www.versioneye.com/user/projects/53619bebfe0d07b45c0000e2/badge.png)](https://www.versioneye.com/user/projects/53619bebfe0d07b45c0000e2)

ReconStruct
===========

Qt 5 tool to help reverse engineer binary file formats, such as game files.

System Dependencies
-------------------

* qt5-base
* libxkbcommon-x11
* PyQt5

ReconStruct needs the python bindings of QHexEdit.

The following needs to be run to install the bindings in /home.

```bash
cd ReconStruct
git submodule init
git submodule update
cd QHexEdit/src
qmake .
make
sudo make install
cd QHexEdit/python
./build.sh
python setup.py install --user
```

Python Dependencies
-------------------
See requirements.txt or run
```bash
pip install -r requirements.txt
```

Note
----
Travis testing has been disabled until [travis-ci.org upgrades to 14.04](https://github.com/travis-ci/travis-ci/issues/2046). This is to avoid building PyQt5 every time.
