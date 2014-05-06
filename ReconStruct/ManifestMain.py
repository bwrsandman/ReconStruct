"""
ReconStruct

Reconstruct is a application which helps easily reverse engineer binary file
formats.  It is tested to run on Python 2.7, 3.3, 3.4 and pypy.

Copyright (c) 2014 Sandy Carter
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

try:
    from ReconStruct.ManifestInt import ManifestInt
    from ReconStruct.ManifestStr import ManifestStr
    from ReconStruct.ManifestCustom import ManifestCustom
except ImportError:
    from ManifestInt import ManifestInt
    from ManifestStr import ManifestStr
    from ManifestCustom import ManifestCustom


class ManifestMain(ManifestCustom):
    """Special version of ManifestCustom which is meant to be the invisible root
    of the manifest tree. Does not repeat and is not immediately accessible
    """
    __manifest_types = {
        'int': ManifestInt,
        'str': ManifestStr,
    }

    def __init__(self):
        super(ManifestMain, self).__init__("Main", 1, None)

    @classmethod
    def get_manifest(cls, item):
        if item in cls.__manifest_types:
            return cls.__manifest_types[item]
        else:
            return ManifestCustom,
