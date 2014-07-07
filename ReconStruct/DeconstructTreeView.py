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
    from ReconStruct.ManifestCustom import ManifestCustom, ParsedCustom
    from ReconStruct.TypesItemDelegate import TypesItemDelegate
except ImportError:
    from ManifestMain import ManifestMain
    from ManifestCustom import ManifestCustom, ParsedCustom
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
        self.header().setStretchLastSection(False)

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
        self.setColumnWidth(self.COL_PREVIEW, 120)

    def on_itemChanged(self, item):
        parent = item.parent() or self.model().invisibleRootItem()
        row = item.index().row()
        label = parent.child(row, self.COL_LABEL).text()
        size = parent.child(row, self.COL_SIZE).text()
        data_type = parent.child(row, self.COL_TYPE).text()
        # adding a custom type
        if data_type == self.tr('custom'):
            data_type, ok = QInputDialog.getText(
                self, self.tr("Create New Type"), self.tr("Name"))
            if not ok:
                return
            self.type_delegate.addType(data_type)
            self.manifest.add_custom_type(data_type)
        ManifestClass = self.manifest.get_manifest(data_type)
        if data_type in self.manifest.saved_manifests:
            children = self.manifest.saved_manifests[data_type].sub_manifests
        else:
            children = []
        selected_row = self.selectionModel().selectedRows()[0]
        manifest, _ = self._get_parent_manifest(selected_row)
        sub_manifests = manifest.sub_manifests
        sub_manifests[row] = ManifestClass(
            label, size, data_type, manifest
        )
        if data_type in self.manifest.saved_manifests:
            sub_manifests[row].sub_manifests = children
        self.refresh_view()

    def on_selectionChanged(self, destination, origin):
        """From selection, go down the tree to get the proper item
        from that item, find result which is in the COL_PREVIEW column
        careful that index rows of custom types don't have COL_PREVIEW column
        """
        try:
            selection_index = destination.indexes()[0]
            parent_index = selection_index.parent()
            row_indices = []
            while parent_index.row() != -1:
                row_indices.append(parent_index.row())
                parent_index = parent_index.parent()
            if row_indices:
                cell = self.model().item(row_indices[-1])
                for row in row_indices[-2::-1] + [selection_index.row()]:
                    cell = cell.child(row)
                if (cell.parent() and
                        cell.parent().columnCount() > self.COL_PREVIEW):
                    cell = cell.parent().child(cell.row(), self.COL_PREVIEW)
            else:
                cell = self.model().item(selection_index.row(),
                                         self.COL_PREVIEW)
        except IndexError:
            cell = self.model().item(0, self.COL_PREVIEW)
        if cell is None:
            start = 0
            end = 0
        elif cell.column() == self.COL_PREVIEW:
            item = cell.result
            start = item.index
            end = start + item.size
        elif cell.hasChildren():
            start = cell.child(0, self.COL_PREVIEW).result.index
            last_result = cell.child(cell.rowCount() - 1,
                                     self.COL_PREVIEW).result
            end = last_result.index + last_result.size
        else:
            start = 0
            end = 0
        self.qHexEdit.setSelection(start, end)

    def _get_parent_manifest(self, selection_index):
        """From selection index, get manifest which matches
        TreeView Rows include indices which are not represented in manifest
        tree structures, so skip them.

        ManifestMain
          Manifests[...]
          Manifest Custom
            Index            (SKIP)
              Manifests[...]
          Manifests[...]

        """
        manifest = self.manifest
        parent_index = selection_index.parent()
        row_indices = []
        while parent_index.row() != -1:
            row_indices.append(parent_index.row())
            parent_index = parent_index.parent()
        # Index was selected, report this
        is_indicator = len(row_indices) % 2 == 1
        # Traverse row indices backwards skipping every other index
        for i in row_indices[::-2]:
            manifest = manifest.sub_manifests[i]
        return manifest, is_indicator

    def add_row(self, label="", size="", data_type="", parent=None):
        """

        :type data_type: str
        """
        manifest = self.manifest
        selected_rows = self.selectionModel().selectedRows()
        index = None
        if len(selected_rows) == 1:
            selected_row = selected_rows[0]
            manifest, _ = self._get_parent_manifest(selected_row)
            index = selected_row.row() + 1
            if not manifest:
                return
        if not size:
            size = str(self.get_selection_size())
        if not data_type:
            data_type = "bytes"
        manifest_class = self.manifest.get_manifest(data_type)
        manifest.add(manifest_class(label, size), index)
        if type(manifest) is not ManifestMain:
            self.manifest.saved_manifests[manifest.type_name] = manifest
        self.refresh_view()

    def remove_row(self):
        requires_refresh = False
        for selection_index in self.selectionModel().selectedRows():
            manifest, is_indicator = self._get_parent_manifest(selection_index)
            if is_indicator:
                continue
            requires_refresh = True
            row = selection_index.row()
            manifest.sub_manifests[row] = None
        if requires_refresh:
            self.refresh_view()

    def refresh_view(self):
        def build_row(result):
            preview = QStandardItem(str(result.data))
            preview.setEditable(False)
            preview.result = result
            return [
                QStandardItem(result.manifest.label),
                QStandardItem(result.manifest._size),
                QStandardItem(result.manifest.type()),
                preview,
            ]

        def build_custom_tree(result, index_item):
            if type(result) is not ParsedCustom:
                return
            parent_item = index_item.child(index_item.rowCount() - 1)
            # Custom nodes have tree representation of their data
            # For the size of custom node, add an index indicator (e.g. [1])
            for i, array_index in enumerate(result.data):
                index_item = QStandardItem("[%d]" % i)
                parent_item.appendRow([index_item])
                # In a level under index indicator, show data types
                for sub_result in array_index:
                    index_item.appendRow(build_row(sub_result))
                    build_custom_tree(sub_result, index_item)
                if i == 0:
                    self.setExpanded(index_item.index(), True)
            self.setExpanded(parent_item.index(), True)

        self.manifest.clean_up()
        root = self.model().invisibleRootItem()
        root.removeRows(0, root.rowCount())
        main_result = self.manifest(bytes(self.qHexEdit.data()))
        model = self.model()
        # Every root level node
        for result in main_result.data[0]:
            root.appendRow(build_row(result))
            build_custom_tree(result, root)
        self.covered_size = main_result.size + main_result.index

    def get_selection_size(self):
        return max(0, self.qHexEdit.cursorPosition() - self.covered_size)
