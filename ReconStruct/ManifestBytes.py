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
    from ReconStruct.ManifestBase import ManifestBase, ParsedBase
except ImportError:
    from ManifestBase import ManifestBase, ParsedBase


class ManifestBytes(ManifestBase):
    """Descriptor manifest which parses bytestrings"""
    def __init__(self, label, size, type_name='bytes', parent=None):
        super(ManifestBytes, self).__init__(label, size, type_name, parent)

    def __call__(self, data, start=0):
        try:
            hex_map = map(hex, ord, data[start:start + self.size])
        except TypeError:  # Python 3
            hex_map = map(hex, data[start:start + self.size])
        preview = "".join(i[2:].zfill(2) for i in hex_map)
        if preview:
            preview = "0x" + preview
        return self.parser()(
            self,
            preview,
            start,
            self.size
        )

    @classmethod
    def type(cls):
        return 'bytes'

    @classmethod
    def parser(cls):
        return ParsedBytes


class ParsedBytes(ParsedBase):
    def __init__(self, manifest, data, index, size):
        super(ParsedBytes, self).__init__(manifest, data, index, size)
