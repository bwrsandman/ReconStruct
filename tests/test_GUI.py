#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
test_ReconStruct
----------------------------------

Tests for `ReconStruct` module.

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

import unittest
import sys
from PyQt5 import QtTest
from PyQt5.QtWidgets import QApplication
from ReconStruct.mainwindow import MainWindow


class TestGUI(unittest.TestCase):
    def setUp(self):
        self.app = QApplication(sys.argv)
        self.window = MainWindow()

    def test_MainWindow(self):
        self.assertTrue(self.window)

    def tearDown(self):
        del self.app
        pass


if __name__ == '__main__':
    unittest.main()
