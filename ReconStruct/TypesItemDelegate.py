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

from PyQt5.QtWidgets import QItemDelegate, QComboBox


class TypesItemDelegate(QItemDelegate):
    def __init__(self, type_names, parent=None):
        super(TypesItemDelegate, self).__init__(parent)
        self.type_names = list(type_names)
        self.custom_types = []

    def createEditor(self, parent, *args):
        editor = QComboBox(parent)
        editor.addItems(self.type_names)
        editor.insertSeparator(len(self.type_names))
        if self.custom_types:
            editor.addItems(self.custom_types)
            editor.insertSeparator(
                len(self.type_names) + len(self.custom_types) + 1)
        editor.addItem(self.tr("custom"))
        return editor

    def addType(self, type_name):
        self.custom_types.append(type_name)
