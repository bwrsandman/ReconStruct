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

import os

from PyQt5.QtWidgets import (
    QDialog,
)
from PyQt5 import uic
from PyQt5.Qt import Qt
import logging

logger = logging.getLogger(__name__)


class FileAttributesWindow(QDialog):
    def __init__(self, manifest, parent=None):
        super(FileAttributesWindow, self).__init__(parent)
        uifile = os.path.join(os.path.dirname(__file__), 'fileattributeswindow.ui')
        uic.loadUi(uifile, self)
        self.manifest = manifest
        self.set_view_from_attrs(manifest.getFileAttributes())

    def set_view_from_attrs(self, attrs):
        for key, val in attrs.items():
            cmb = getattr(self, 'cmb_' + key)
            index = cmb.findText(self.tr(val), Qt.MatchFixedString)
            if index == -1:
                logger.warn(
                    'Could not set attribute %s (%s) in attribute view.',
                    self.tr(val), val
                )
                continue
            cmb.setCurrentIndex(index)

    def get_attrs_from_view(self):
        res = {}
        keys = [a[4:] for a in self.__dir__() if a.startswith('cmb_')]
        for key in keys:
            cmb = getattr(self, 'cmb_' + key)
            res[key] = cmb.currentText().lower()
        return res

    def accept(self):
        self.manifest.setFileAttributes(self.get_attrs_from_view())
        return super(FileAttributesWindow, self).accept()
