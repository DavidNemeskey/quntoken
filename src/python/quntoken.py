#! /usr/bin/env python3
# -*- coding: utf-8, vim: expandtab:ts=4 -*-

"""Sentence splitter and tokenizer for hungarian texts in utf-8."""

import contextlib
import ctypes
import os.path as op

lib = ctypes.cdll.LoadLibrary(
    op.join(op.dirname(op.abspath(__file__)), 'quntoken_py.so'))

"""
Class interface to the quntoken library, a sentence splitter and tokenizer for
hungarian texts in utf-8.
"""
class QunToken(object):
    def __init__(self, format, mode, hyphen):
        """
        :param format: the output format.
                       One of ``xml``, ``json``, ``vert`` and ``raw``.
        :param mode: the modus operandi. One of ``sentence`` and ``token``. Both
                     perform sentence splitting; the latter tokenizes the text
                     as well.
        :param hyphen: whether to eliminate word breaks from the end of lines.
        """
        lib.QunToken_new.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool]
        lib.QunToken_new.restype = ctypes.c_void_p
        lib.QunToken_tokenize.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        lib.QunToken_tokenize.restype = ctypes.c_char_p
        self.obj = lib.QunToken_new(
            format.encode('utf-8'), mode.encode('utf-8'), hyphen)
        if not self.obj:
            raise ValueError('Wrong format: {}. '.format(format) +
                             'Valid formats: xml, json, vert, raw.')

    def tokenize(self, input):
        """Tokenizes *input* and returns a string."""
        return lib.QunToken_tokenize(
            self.obj, input.encode('utf-8')).decode('utf-8')


def main():
    pass

if __name__ == '__main__':
    main()
