#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
test_Manifest
----------------------------------

Tests for Manifest files of `ReconStruct` module.

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
from ReconStruct.ManifestStr import ManifestStr
from ReconStruct.ManifestInt import ManifestInt
from ReconStruct.ManifestCustom import ManifestCustom


class TestManifest(unittest.TestCase):
    def setUp(self):
        self.data = b"""HELLO WORLD!"""

    def testInt(self):
        data = b"\x00\x00\x00\x00\x10"
        interpreter = ManifestInt("Test", 5)
        result = interpreter(data)
        self.assertEqual(result.index, 0)
        self.assertEqual(result.size, 5)
        self.assertEqual(result.data, 16)

    def testStr(self):
        interpreter = ManifestStr("Test", 5)
        result = interpreter(self.data)
        self.assertEqual(result.index, 0)
        self.assertEqual(result.size, 5)
        self.assertEqual(result.data, u"HELLO")

    def testStrBadSize(self):
        interpreter = ManifestStr("Test", "BadKey")
        result = interpreter(self.data)
        self.assertEqual(result.index, 0)
        self.assertEqual(result.size, 0)
        self.assertEqual(result.data, u"")

    def testCustomSizeOne(self):
        interpreter = ManifestCustom("Test Custom", 1, "test_type")
        interpreter.add(ManifestStr("Test String Hello", 5))
        interpreter.add(ManifestStr("Test String space", 1))
        interpreter.add(ManifestStr("Test String world", 5))
        result = interpreter(self.data)
        self.assertEqual(result.data[0][0].index, 0)
        self.assertEqual(result.data[0][0].size, 5)
        self.assertEqual(result.data[0][0].data, u"HELLO")
        self.assertEqual(result.data[0][1].index, 5)
        self.assertEqual(result.data[0][1].size, 1)
        self.assertEqual(result.data[0][1].data, u" ")
        self.assertEqual(result.data[0][2].index, 6)
        self.assertEqual(result.data[0][2].size, 5)
        self.assertEqual(result.data[0][2].data, u"WORLD")
        self.assertEqual(result.size, 11)
        self.assertEqual(result.index, 0)

    def testCustomSizeTwo(self):
        interpreter = ManifestCustom("Test Custom", 2, "test_type")
        interpreter.add(ManifestStr("Test String word", 5))
        interpreter.add(ManifestStr("Test String special", 1))
        result = interpreter(self.data)
        self.assertEqual(result.data[0][0].index, 0)
        self.assertEqual(result.data[0][0].size, 5)
        self.assertEqual(result.data[0][0].data, u"HELLO")
        self.assertEqual(result.data[0][1].index, 5)
        self.assertEqual(result.data[0][1].size, 1)
        self.assertEqual(result.data[0][1].data, u" ")
        self.assertEqual(result.data[1][0].index, 6)
        self.assertEqual(result.data[1][0].size, 5)
        self.assertEqual(result.data[1][0].data, u"WORLD")
        self.assertEqual(result.data[1][1].index, 11)
        self.assertEqual(result.data[1][1].size, 1)
        self.assertEqual(result.data[1][1].data, u"!")
        self.assertEqual(result.size, 12)
        self.assertEqual(result.index, 0)

    def testIntStrInterpretation(self):
        data = b"\x0CHELLO WORLD!\x27This will test variable length strings."
        interpreter = ManifestCustom("One Int, variable str", 2, "test_type")
        interpreter.add(ManifestInt("StrLen", 1))
        interpreter.add(ManifestStr("Str", "StrLen"))
        result = interpreter(data)
        self.assertEqual(result.data[0][1].index, 1)
        self.assertEqual(result.data[0][1].size, 0x0C)
        self.assertEqual(result.data[0][1].data, u"HELLO WORLD!")
        self.assertEqual(result.data[1][1].index, 0x0E)
        self.assertEqual(result.data[1][1].size, 0x27)
        self.assertEqual(result.data[1][1].data, u"This will test variable length strings.")
        self.assertEqual(result.size, len(data))
        self.assertEqual(result.index, 0)

    def testCustomBadSize(self):
        interpreter = ManifestCustom("One String in Custom with key to nonexistant size", 2, "test_type")
        interpreter.add(ManifestStr("Test", "BadKey"))
        result = interpreter(self.data)
        self.assertEqual(result.data[0][0].index, 0)
        self.assertEqual(result.data[0][0].size, 0)
        self.assertEqual(result.data[0][0].data, u"")
        self.assertEqual(result.data[1][0].index, 0)
        self.assertEqual(result.data[1][0].size, 0)
        self.assertEqual(result.data[1][0].data, u"")
        self.assertEqual(result.size, 0)
        self.assertEqual(result.index, 0)



    def testIntCustomInterpretation(self):
        data = b"""\x03\x04\x07This test takes seven slices of 3 and 4 length words"""
        interpreter = ManifestCustom("String slices", 1, "test_type")
        interpreter.add(ManifestInt("StrLen1", 1))
        interpreter.add(ManifestInt("StrLen2", 1))
        interpreter.add(ManifestInt("ArrayLen", 1))
        array = ManifestCustom("Array", "ArrayLen", "test_array")
        array.add(ManifestStr("String", "StrLen1"))
        array.add(ManifestStr("String", "StrLen2"))
        interpreter.add(array)
        result = interpreter(data)
        slices = "Thi;s te|st ;take|s s;even| sl;ices| of; 3 a|nd ;4 le|ngt;h wo|rds".split('|')[:7]
        slices = list(map(lambda x: x.split(';'), slices))
        self.assertEqual(result.data[0][0].data, 3)
        self.assertEqual(result.data[0][1].data, 4)
        self.assertEqual(result.data[0][2].data, 7)
        result_array = result.data[0][3]
        self.assertEqual([[y.data for y in x] for x in result_array.data], slices)
        self.assertEqual(result.size, 3 + (3 + 4) * 7)

    def testConditionalSize(self):
        dataFalse = b"\x10\x04Test"
        dataTrue = b"\x01\x04Test"
        interpreter = ManifestCustom("TestConditional", 1, "test_type")
        interpreter.add(ManifestInt("Condition", 1))
        interpreter.add(ManifestInt("StrSize", "Condition < 5"))
        interpreter.add(ManifestStr("String", "StrSize"))

        result_false = interpreter(dataFalse)
        result_true = interpreter(dataTrue)

        self.assertEqual(result_false.size, 1)
        self.assertEqual(result_false.data[0][0].data, 16)
        self.assertEqual(result_false.data[0][1].data, 0)
        self.assertEqual(result_false.data[0][2].data, u"")
        self.assertEqual(result_true.size, 6)
        self.assertEqual(result_true.data[0][0].data, 1)
        self.assertEqual(result_true.data[0][1].data, 4)
        self.assertEqual(result_true.data[0][2].data, u"Test")

    def tearDown(self):
        pass


if __name__ == '__main__':
    unittest.main()
