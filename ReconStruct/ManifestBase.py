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


class ManifestBase(object):
    """Base class for all descriptions of data representation

    Provide :func:`size`, :func:`__init__`, for this family of classes.
    """
    def __init__(self, label, size, parent=None):
        self.label = label
        self._size = size
        self.parent = parent

    @property
    def size(self):
        """Return dynamic size of Manifest.

        If ``type(self._size) is not int`` the size is dependent on a
        previously parsed value and this function will fetch it. If not,
        then get size directly from ``self._size``.
        """
        ret = 0
        if type(self._size) is int:
            ret = self._size
        elif self.parent:
            ret = self.parent.actual_size_of(self._size)
        else:
            ret = 0
        assert type(ret) in (int, bool), u"ret is of type %s" % type(ret)
        return ret
