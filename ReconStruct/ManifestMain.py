# -*- encoding: utf-8 -*-
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
    from ReconStruct.ManifestBool import ManifestBool
    from ReconStruct.ManifestStr import ManifestStr
    from ReconStruct.ManifestBytes import ManifestBytes
    from ReconStruct.ManifestCustom import ManifestCustom
except ImportError:
    from ManifestInt import ManifestInt
    from ManifestBool import ManifestBool
    from ManifestStr import ManifestStr
    from ManifestBytes import ManifestBytes
    from ManifestCustom import ManifestCustom


class ManifestMain(ManifestCustom):
    """Special version of ManifestCustom which is meant to be the invisible root
    of the manifest tree. Does not repeat and is not immediately accessible
    """
    __manifest_types = {
        ManifestClass.type(): ManifestClass for ManifestClass in (
            ManifestInt,
            ManifestBool,
            ManifestStr,
            ManifestBytes,
        )
    }

    compression = None

    def __init__(self):
        super(ManifestMain, self).__init__("Main", 1, 'main', None)
        self.saved_manifests = dict()

    def add(self, manifest):
        """Add child manifest to saved_manifests

        :param manifest: A new child of this manifest.
        :type manifest: ManifestBase.
        """
        super(ManifestMain, self).add(manifest)
        if (manifest.type() not in self.__manifest_types.keys()
                and manifest.type() != 'main'):
            self.saved_manifests[manifest.type_name] = manifest

    def get_manifest(self, item):
        if item in self.__manifest_types:
            return self.__manifest_types[item]
        else:
            return ManifestCustom

    @classmethod
    def get_type_names(cls):
        return cls.__manifest_types.keys()

    def add_custom_type(self, name):
        self.__manifest_types[name] = self.__manifest_types.get(
            name, ManifestCustom
        )

    def setFileAttributes(self, attrs):
        self.compression = attrs.get('compression')
        endianness = attrs.get('endianness')
        if endianness is not None:
            if endianness not in ('little', 'big'):
                raise ValueError('Unexpected endianness: %s' % endianness)
            self.byteorder = endianness

    def getFileAttributes(self):
        attrs = {
            'endianness': self.byteorder,
        }

        if self.compression:
            attrs['compression'] = self.compression

        return attrs

