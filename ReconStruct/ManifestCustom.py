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
    from ReconStruct.ManifestBase import ManifestBase
except ImportError:
    from ManifestBase import ManifestBase


class ManifestCustom(ManifestBase):
    """Descriptior manifest which can contain other manifests.
    Allows repeating datatypes to be represented in an array-style fashion
    Provide :func:`add` and :func:`actual_size_of`
    """
    def __init__(self, label, size, parent=None):
        super(ManifestCustom, self).__init__(label, size, parent)
        self.submanifests = []
        self._current_data = []

    def add(self, manifest):
        """Add child manifests to this manifest and set their parent

        :param manifest: A new child of this manifest.
        :type manifest: ManifestBase.
        """
        self.submanifests.append(manifest)
        manifest.parent = self

    def actual_size_of(self, label):
        """Search for pre-computed and cached size for label in self and parents
        :func:`__call__` stashes values temporarily in ``self._current_data``
        if not data is found return 0

        :param label: The label of size field.
        :type label: str.
        :returns:  int -- the parsed size.
        """
        if label in self._current_data:
            ret = self._current_data[label]
        elif self.parent:
            ret = self.parent.actual_size_of(label)
        else:
            ret = 0
        assert type(ret) is int
        return ret

    def __call__(self, data, start=0):
        """Parse data with child manifests caching computed values in
        ``self._current_data`` in case they are pooled later for field sizes.

        :param data: The data being parsed.
        :type data: bytes.
        :param size: The index at which the parsing is at.
        :type start: int.
        """
        ret = []
        parsed = -start
        for _ in range(self.size):
            self._current_data = dict()
            sub_ret = []
            for manifest in self.submanifests:
                formatted, shift = manifest(data, start)
                sub_ret.append(formatted)
                start += shift
                self._current_data[manifest.label] = formatted
            ret.append(sub_ret)
        del self._current_data
        return ret, parsed + start
