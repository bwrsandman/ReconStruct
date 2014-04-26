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

    def testInt(self):
        data = b"\x00\x00\x00\x00\x10"
        interpreter = ManifestInt("Test", 5)

        self.assertEqual(interpreter(data), (16, 5))

    def testStr(self):
        interpreter = ManifestStr("Test", 5)

        self.assertEqual(interpreter(self.data), (u"HELLO", 5))

    def testStrBadSize(self):
        interpreter = ManifestStr("Test", "BadKey")

        self.assertEqual(interpreter(self.data), (u"", 0))

    def testCustomSizeOne(self):
        interpreter = ManifestCustom("Test Custom", 1)
        interpreter.add(ManifestStr("Test String Hello", 5))
        interpreter.add(ManifestStr("Test String space", 1))
        interpreter.add(ManifestStr("Test String world", 5))

        self.assertEqual(interpreter(self.data),
                         ([[u"HELLO", u" ", u"WORLD"]], 11))

    def testCustomSizeTwo(self):
        interpreter = ManifestCustom("Test Custom", 2)
        interpreter.add(ManifestStr("Test String word", 5))
        interpreter.add(ManifestStr("Test String special", 1))

        self.assertEqual(interpreter(self.data),
                         ([[u"HELLO", u" "], [u"WORLD", u"!"]], 12))

    def testIntStrInterpretation(self):
        data = b"""\x0CHELLO WORLD!\x27This will test variable length strings."""
        interpreter = ManifestCustom("One Int, variable String", 2)
        interpreter.add(ManifestInt("StrLen", 1))
        interpreter.add(ManifestStr("Str", "StrLen"))

        interpreted, parsed = interpreter(data)

        self.assertEqual(interpreted[0], [0x0C, "HELLO WORLD!"])
        self.assertEqual(interpreted[1], [0x27, "This will test variable length strings."])
        self.assertEqual(parsed, len(data))

    def testCustomBadSize(self):
        interpreter = ManifestCustom("One String in Custom with key to nonexistant size", 2)
        interpreter.add(ManifestStr("Test", "BadKey"))

        self.assertEqual(interpreter(self.data), (2 * [[u""]], 0))


    def testIntCustomInterpretation(self):
        data = b"""\x03\x04\x07This test takes seven slices of 3 and 4 length words"""
        interpreter = ManifestCustom("String slices", 1)
        interpreter.add(ManifestInt("StrLen1", 1))
        interpreter.add(ManifestInt("StrLen2", 1))
        interpreter.add(ManifestInt("ArrayLen", 1))
        array = ManifestCustom("Array", "ArrayLen")
        array.add(ManifestStr("String", "StrLen1"))
        array.add(ManifestStr("String", "StrLen2"))
        interpreter.add(array)

        interpreted, parsed = interpreter(data)

        slices = "Thi;s te|st ;take|s s;even| sl;ices| of; 3 a|nd ;4 le|ngt;h wo|rds".split('|')[:7]
        slices = list(map(lambda x: x.split(';'), slices))
        self.assertEqual(interpreted[0], [3, 4, 7, slices])
        self.assertEqual(parsed, 3 + (3 + 4) * 7)

    def tearDown(self):
        pass


if __name__ == '__main__':
    unittest.main()
