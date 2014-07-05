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
    QMainWindow,
    QFileDialog,
    QApplication,
    QErrorMessage,
)
from PyQt5.QtCore import pyqtSlot, Qt
from PyQt5.QtGui import QCursor
from PyQt5 import uic

try:
    from ReconStruct.SchemaIO import Markdown
    from ReconStruct.DeconstructTreeView import DeconstructTreeView
except ImportError:
    from SchemaIO import Markdown
    from DeconstructTreeView import DeconstructTreeView


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        uifile = os.path.join(os.path.dirname(__file__), 'mainwindow.ui')
        uic.loadUi(uifile, self)
        self._current_filename = None
        self.treeView = DeconstructTreeView(self.qHexEdit, self.splitter)
        self.show()

    @pyqtSlot()
    def on_action_Open_triggered(self):
        filename, filter = QFileDialog.getOpenFileName(
            self, self.tr(u"Open Binary File"), '.')
        if filename:
            self.loadBinary(filename)

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

    def loadBinary(self, filename):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        with open(filename, 'rb') as f:
            self.qHexEdit.setData(f.read())
        QApplication.restoreOverrideCursor()
        self._current_filename = filename

    def saveBinary(self, filename):
        pass

    def loadSchema(self, filename):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        try:
            with open(filename, 'r') as f:
                self.treeView.manifest = Markdown.fromMarkdown(f.read())
        except ValueError as e:
            QErrorMessage(self).showMessage(
                self.tr('Format Error: ') + str(e)
            )
        finally:
            self.treeView.refresh_view()
            QApplication.restoreOverrideCursor()

    def saveSchema(self, filename):
        QApplication.setOverrideCursor(QCursor(Qt.WaitCursor))
        with open(filename, 'w') as f:
            f.write(Markdown.toMarkdown(self.treeView.manifest))
        QApplication.restoreOverrideCursor()

    def addRow(self, label="", size="", data_type="", parent=None):
        self.treeView.add_row(label, size, data_type, parent)
