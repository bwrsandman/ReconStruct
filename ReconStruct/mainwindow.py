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
from difflib import SequenceMatcher

import os

from PyQt5.QtWidgets import (
    QMainWindow,
    QFileDialog,
    QApplication,
    QErrorMessage,
    QLabel
)
from PyQt5.QtCore import pyqtSlot, Qt
from PyQt5.QtGui import QCursor
from PyQt5 import uic
import logging
import gzip
import bz2

try:
    from ReconStruct.SchemaIO import Markdown
    from ReconStruct.DeconstructTreeView import DeconstructTreeView
    from ReconStruct.fileattributeswindow import FileAttributesWindow
except ImportError:
    from SchemaIO import Markdown
    from DeconstructTreeView import DeconstructTreeView
    from fileattributeswindow import FileAttributesWindow


logger = logging.getLogger(__name__)
STATUSBAR_TIMEOUT = 2000


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        uifile = os.path.join(os.path.dirname(__file__), 'mainwindow.ui')
        uic.loadUi(uifile, self)
        self._current_filename = None
        self._current_schema_filename = None
        self.treeView = DeconstructTreeView(self.qHexEdit, self.splitter)
        self.lblSelection = QLabel()
        self.lblRange = QLabel()
        self.lblOffset = QLabel()
        self.statusBar.insertPermanentWidget(0, self.lblSelection)
        self.statusBar.insertPermanentWidget(1, self.lblRange)
        self.statusBar.insertPermanentWidget(2, self.lblOffset)
        self.qHexEdit.currentAddressChanged.connect(self.currentAddressChanged)
        self.refreshTitle()
        self.show()

    @pyqtSlot()
    def on_action_Open_triggered(self):
        filename, filter = QFileDialog.getOpenFileName(
            self, self.tr(u"Open Binary File"), '.')
        if filename:
            self.loadBinary(filename)

    @pyqtSlot()
    def on_action_Reload_triggered(self):
        if self._current_filename:
            self.refreshBinary()

    @pyqtSlot()
    def on_action_Save_triggered(self):
        filename, filter = QFileDialog.getSaveFileName(
            self, self.tr(u"Save Binary File"), '.')
        if filename:
            self.saveBinary(filename)

    @pyqtSlot()
    def on_action_Import_Schema_triggered(self):
        filename, filter = QFileDialog.getOpenFileName(
            self, self.tr(u"Open Schema of Binary File"), '.')
        if filename:
            self.loadSchema(filename)

    @pyqtSlot()
    def on_action_Export_Schema_triggered(self):
        filename, filter = QFileDialog.getSaveFileName(
            self, self.tr(u"Save Schema of Binary File"), '.')
        if filename:
            self.saveSchema(filename)

    @pyqtSlot()
    def on_action_Add_triggered(self):
        self.addRow()

    @pyqtSlot()
    def on_action_Remove_triggered(self):
        self.removeRow()

    @pyqtSlot()
    def on_action_Attributes_triggered(self):
        def on_quit():
            if self._current_filename:
                self.loadBinary(self._current_filename)
        attr_window = FileAttributesWindow(self.treeView.manifest,
                                           on_quit=on_quit, parent=self)
        attr_window.show()

    def loadBinary(self, filename):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        if self.treeView.manifest.compression == 'gzip':
            reader = gzip.open
        elif self.treeView.manifest.compression == 'bz2':
            reader = bz2.BZ2File
        else:
            reader = open
        try:
            with reader(filename, 'rb') as f:
                data = f.read()
            self.qHexEdit.setData(data)
            self._current_filename = filename
            self.statusBar.showMessage(
                self.tr("Loaded binary: ") + os.path.basename(filename),
                STATUSBAR_TIMEOUT
            )
            self.refreshTitle()
        except IOError as e:
            QErrorMessage(self).showMessage(
                self.tr('Format Error: ') + str(e)
            )
            logger.exception(e)
        finally:
            self.treeView.refresh_view()
            QApplication.restoreOverrideCursor()


    def refreshBinary(self):
        old_data = bytes(self.qHexEdit.data())
        self.loadBinary(self._current_filename)
        new_data = bytes(self.qHexEdit.data())
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        # Long call (1 second)
        diff_op_codes = SequenceMatcher(None, old_data, new_data).get_opcodes()
        delete_indices = []
        replace_ranges = []
        insert_ranges = []
        for op, old1, old2, new1, new2 in diff_op_codes:
            if op == 'delete':
                delete_indices.append(new1)
            elif op == 'replace':
                replace_ranges.append((new1, new2))
            elif op == 'insert':
                insert_ranges.append((new1, new2))
        for start, end in replace_ranges + insert_ranges:
            self.qHexEdit.setHighlightedRange(start, end)
        QApplication.restoreOverrideCursor()

    def saveBinary(self, filename):
        pass

    def refreshTitle(self):
        title = " - ".join(map(os.path.basename, filter(None, (
            self._current_filename,
            self._current_schema_filename,
        )))) + " - ReconStruct"
        self.setWindowTitle(title)

    def loadSchema(self, filename):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        try:
            with open(filename, 'r') as f:
                self.treeView.manifest = Markdown.fromMarkdown(f.read(), self)
        except ValueError as e:
            QErrorMessage(self).showMessage(
                self.tr('Format Error: ') + str(e)
            )
            logger.exception(e)
        else:
            self._current_schema_filename = filename
            self.statusBar.showMessage(
                self.tr("Loaded schema: ") + os.path.basename(filename),
                STATUSBAR_TIMEOUT
            )
            self.refreshTitle()
        finally:
            if self._current_filename:
                self.loadBinary(self._current_filename)
            self.treeView.refresh_view()
            QApplication.restoreOverrideCursor()

    def saveSchema(self, filename):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        try:
            out = Markdown.toMarkdown(self.treeView.manifest)
            with open(filename, 'w') as f:
                f.write(out)
        except Exception as e:
            QErrorMessage(self).showMessage(
                self.tr('Error while saving: ') + str(e)
            )
            logger.exception(e)
        finally:
            QApplication.restoreOverrideCursor()

    def addRow(self, label="", size="", data_type="", parent=None):
        self.treeView.add_row(label, size, data_type, parent)

    def removeRow(self):
        self.treeView.remove_row()

    def currentAddressChanged(self):
        """Display selection information and cursor location
        Hide selection info if nothing is selected"""
        selection = self.qHexEdit.selectionSize()
        start = self.qHexEdit.selectionStart()
        end = self.qHexEdit.selectionEnd()
        offset = self.qHexEdit.cursorPosition()
        self.lblSelection.setText(self.tr('Selection: 0x%X') % selection)
        self.lblRange.setText(self.tr('Range: 0x%X - 0x%X') % (start, end))
        self.lblOffset.setText(self.tr('Offset: 0x%X') % offset)
        self.lblSelection.setVisible(selection)
        self.lblRange.setVisible(selection)
