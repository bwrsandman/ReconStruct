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

from PyQt5.QtCore import QItemSelectionModel
from PyQt5.QtGui import QStandardItem, QStandardItemModel
from PyQt5.QtWidgets import QTreeView, QAbstractItemView

try:
    from ReconStruct.ManifestMain import ManifestMain
except ImportError:
    from ManifestMain import ManifestMain


class DeconstructTreeView(QTreeView):

    COL_LABEL = 0
    COL_SIZE = 1
    COL_TYPE = 2
    COL_PREVIEW = 3

    def __init__(self, qHexEdit, parent=None):
        super(DeconstructTreeView, self).__init__(parent)

        self.qHexEdit = qHexEdit

        self.manifest = ManifestMain()
        self.coveredSize = 0

        self.setAlternatingRowColors(True)
        self.setSelectionMode(QAbstractItemView.ContiguousSelection)

        model = QStandardItemModel()
        model.setHorizontalHeaderItem(
            self.COL_LABEL, QStandardItem(self.tr("Label")))
        model.setHorizontalHeaderItem(
            self.COL_SIZE, QStandardItem(self.tr("Size")))
        model.setHorizontalHeaderItem(
            self.COL_TYPE, QStandardItem(self.tr("Type")))
        model.setHorizontalHeaderItem(
            self.COL_PREVIEW, QStandardItem(self.tr("Preview")))

        self.setModel(model)
        self.setSelectionModel(QItemSelectionModel(self.model(), self))

        self.setColumnWidth(self.COL_LABEL, 100)
        self.setColumnWidth(self.COL_SIZE, 50)
        self.setColumnWidth(self.COL_TYPE, 55)
        self.setColumnWidth(self.COL_PREVIEW, 20)

    def addRow(self, label="", size="", data_type="", parent=None):
        """

        :type data_type: str
        """
        if not parent:
             parent = self.model().invisibleRootItem()
        if not size:
            # size = hex(self.getSelectionSize())
            size = str(self.getSelectionSize())
        if not data_type:
            data_type = "bytes"
        ManifestClass = ManifestMain.get_manifest(data_type)
        self.manifest.add(ManifestClass(label, size))
        self.refreshView()

    def refreshView(self):
        root = self.model().invisibleRootItem()
        root.removeRows(0, root.rowCount())
        interp, size = self.manifest(bytes(self.qHexEdit.data()))
        for manifest, data in zip(self.manifest, interp[0]):
            parent = root
            preview = QStandardItem(data)
            preview.setEditable(False)
            parent.appendRow([
                QStandardItem(manifest.label),
                QStandardItem(manifest._size),
                QStandardItem(manifest.type()),
                preview,
            ])
        self.coveredSize = size

    def getSelectionSize(self):
        return max(0, self.qHexEdit.cursorPosition() - self.coveredSize)
