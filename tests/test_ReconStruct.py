#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
test_ReconStruct
----------------------------------

Tests for `ReconStruct` module.
"""

import unittest
from ReconStruct.ManifestStr import ManifestStr
from ReconStruct.ManifestInt import ManifestInt
from ReconStruct.ManifestCustom import ManifestCustom


class TestHello(unittest.TestCase):
    def setUp(self):
        self.data = b"""HELLO WORLD!"""

    def testStr(self):
        interpreter = ManifestStr("Test", 5)
        self.assertEqual(interpreter(self.data)[0], u"HELLO")

    def testCustomSizeOne(self):
        interpreter = ManifestCustom("Test Custom", 1)
        interpreter.add(ManifestStr("Test String Hello", 5))
        interpreter.add(ManifestStr("Test String space", 1))
        interpreter.add(ManifestStr("Test String world", 5))
        self.assertEqual(interpreter(self.data)[0],
                         [[u"HELLO", u" ", u"WORLD"]])

    def testCustomSizeTwo(self):
        interpreter = ManifestCustom("Test Custom", 2)
        interpreter.add(ManifestStr("Test String word", 5))
        interpreter.add(ManifestStr("Test String special", 1))
        self.assertEqual(interpreter(self.data)[0],
                         [[u"HELLO", u" "], [u"WORLD", u"!"]])

    def tearDown(self):
        pass


class TestIntStr(unittest.TestCase):
    def setUp(self):
        self.data = b"""\x0CHELLO WORLD!\x27This will test variable length strings."""
        self.interpreter = ManifestCustom("One Int, variable String", 2)
        self.interpreter.add(ManifestInt("StrLen", 1))
        self.interpreter.add(ManifestStr("Str", "StrLen"))

    def testIntStrInterpretation(self):
        interpreted, parsed = self.interpreter(self.data)
        self.assertEqual(parsed, len(self.data))
        self.assertEqual(interpreted[0], [0x0C, "HELLO WORLD!"])
        self.assertEqual(interpreted[1], [0x27, "This will test variable length strings."])

    def tearDown(self):
        pass


if __name__ == '__main__':
    unittest.main()
