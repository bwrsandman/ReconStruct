# -*- coding: utf-8 -*-
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
from PyQt5.QtWidgets import QTreeView, QAbstractItemView, QInputDialog

try:
    from ReconStruct.ManifestMain import ManifestMain
    from ReconStruct.TypesItemDelegate import TypesItemDelegate
except ImportError:
    from ManifestMain import ManifestMain
    from TypesItemDelegate import TypesItemDelegate


class DeconstructTreeView(QTreeView):

    COL_LABEL = 0
    COL_SIZE = 1
    COL_TYPE = 2
    COL_PREVIEW = 3

    def __init__(self, qHexEdit, parent=None):
        super(DeconstructTreeView, self).__init__(parent)

        self.qHexEdit = qHexEdit

        self.manifest = ManifestMain()
        self.covered_size = 0

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

        selection_model = QItemSelectionModel(model, self)
        model.itemChanged.connect(self.on_itemChanged)
        selection_model.selectionChanged.connect(self.on_selectionChanged)

        self.setModel(model)
        self.setSelectionModel(selection_model)

        self.type_delegate = TypesItemDelegate(
            ManifestMain.get_type_names(), parent=self)
        self.setItemDelegateForColumn(self.COL_TYPE, self.type_delegate)

        self.setColumnWidth(self.COL_LABEL, 100)
        self.setColumnWidth(self.COL_SIZE, 50)
        self.setColumnWidth(self.COL_TYPE, 55)
        self.setColumnWidth(self.COL_PREVIEW, 20)

    def on_itemChanged(self, item):
        row = item.index().row()
        model = self.model()
        label = model.item(row, self.COL_LABEL).text()
        size = model.item(row, self.COL_SIZE).text()
        data_type = model.item(row, self.COL_TYPE).text()
        if data_type == self.tr('custom'):
            data_type, ok = QInputDialog.getText(
                self, self.tr("Create New Type"), self.tr("Name"))
            if not ok:
                return
            self.type_delegate.addType(data_type)
            self.manifest.add_custom_type(data_type)
        ManifestClass = ManifestMain.get_manifest(data_type)
        self.manifest.sub_manifests[row] = ManifestClass(
            label, size, data_type, self.manifest)
        self.refresh_view()

    def on_selectionChanged(self, destination, origin):
        try:
            row = destination.indexes()[0].row()
        except IndexError:
            row = 0
        item = self.model().item(row, self.COL_PREVIEW).result
        self.qHexEdit.setSelection(item.index, item.index + item.size)

    def add_row(self, label="", size="", data_type="", parent=None):
        """

        :type data_type: str
        """
        if not parent:
            parent = self.model().invisibleRootItem()
        if not size:
            # size = hex(self.getSelectionSize())
            size = str(self.get_selection_size())
        if not data_type:
            data_type = "bytes"
        ManifestClass = ManifestMain.get_manifest(data_type)
        self.manifest.add(ManifestClass(label, size))
        self.refresh_view()

    def refresh_view(self):
        root = self.model().invisibleRootItem()
        root.removeRows(0, root.rowCount())
        main_result = self.manifest(bytes(self.qHexEdit.data()))
        for manifest, result in zip(self.manifest, main_result.data[0]):
            parent = root
            preview = QStandardItem(str(result.data))
            preview.setEditable(False)
            preview.result = result
            parent.appendRow([
                QStandardItem(manifest.label),
                QStandardItem(manifest._size),
                QStandardItem(manifest.type()),
                preview,
            ])
        self.covered_size = main_result.size + main_result.index

    def get_selection_size(self):
        return max(0, self.qHexEdit.cursorPosition() - self.covered_size)
