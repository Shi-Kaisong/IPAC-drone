#!/usr/bin/env python

# Python 2/3 compatibility
from __future__ import print_function

import os, numpy as np

import cv2 as cv

from tests_common import NewOpenCVTests

class aruco_test(NewOpenCVTests):

    def test_idsAccessibility(self):

        ids = np.arange(17)
        rev_ids = ids[::-1]

        aruco_dict  = cv.aruco.Dictionary_get(cv.aruco.DICT_5X5_250)
        board = cv.aruco.CharucoBoard_create(7, 5, 1, 0.5, aruco_dict)

        np.testing.assert_array_equal(board.ids.squeeze(), ids)

        board.ids = rev_ids
        np.testing.assert_array_equal(board.ids.squeeze(), rev_ids)

        board.setIds(ids)
        np.testing.assert_array_equal(board.ids.squeeze(), ids)

        with self.assertRaises(cv.error):
            board.setIds(np.array([0]))

    def test_drawCharucoDiamond(self):
        aruco_dict = cv.aruco.Dictionary_get(cv.aruco.DICT_4X4_50)
        img = cv.aruco.drawCharucoDiamond(aruco_dict, np.array([0, 1, 2, 3]), 100, 80)
        self.assertTrue(img is not None)

    def test_write_read_dict(self):

        try:
            aruco_dict = cv.aruco.getPredefinedDictionary(cv.aruco.DICT_5X5_50)
            markers_gold = aruco_dict.bytesList

            # write aruco_dict
            filename = "test_dict.yml"
            fs_write = cv.FileStorage(filename, cv.FileStorage_WRITE)
            aruco_dict.writeDictionary(fs_write)
            fs_write.release()

            # reset aruco_dict
            aruco_dict = cv.aruco.getPredefinedDictionary(cv.aruco.DICT_6X6_250)

            # read aruco_dict
            fs_read = cv.FileStorage(filename, cv.FileStorage_READ)
            aruco_dict.readDictionary(fs_read.root())
            fs_read.release()

            # check equal
            self.assertEqual(aruco_dict.markerSize, 5)
            self.assertEqual(aruco_dict.maxCorrectionBits, 3)
            np.testing.assert_array_equal(aruco_dict.bytesList, markers_gold)

        finally:
            if os.path.exists(filename):
                os.remove(filename)

    def test_identify(self):
        aruco_dict = cv.aruco.Dictionary_get(cv.aruco.DICT_4X4_50)
        expected_idx = 9
        expected_rotation = 2
        bit_marker = np.array([[0, 1, 1, 0], [1, 0, 1, 0], [1, 1, 1, 1], [0, 0, 1, 1]], dtype=np.uint8)

        check, idx, rotation = aruco_dict.identify(bit_marker, 0)

        self.assertTrue(check, True)
        self.assertEqual(idx, expected_idx)
        self.assertEqual(rotation, expected_rotation)

    def test_getDistanceToId(self):
        aruco_dict = cv.aruco.Dictionary_get(cv.aruco.DICT_4X4_50)
        idx = 7
        rotation = 3
        bit_marker = np.array([[0, 1, 0, 1], [0, 1, 1, 1], [1, 1, 0, 0], [0, 1, 0, 0]], dtype=np.uint8)
        dist = aruco_dict.getDistanceToId(bit_marker, idx)

        self.assertEqual(dist, 0)

if __name__ == '__main__':
    NewOpenCVTests.bootstrap()
