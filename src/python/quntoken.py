#! /usr/bin/env python3
# -*- coding: utf-8, vim: expandtab:ts=4 -*-

"""Sentence splitter and tokenizer for hungarian texts in utf-8."""

import ctypes
import os.path as op
import sys

lib = ctypes.cdll.LoadLibrary(
    op.join(op.dirname(op.abspath(__file__)), 'quntoken_py.so'))

"""
Class interface to the quntoken library, a sentence splitter and tokenizer for
hungarian texts in utf-8.
"""
class QunToken(object):
    # Python 2 and 3 compatibility (without future, past, six, etc.)
    str_class = str if sys.version_info.major >= 3 else unicode

    def __init__(self, format, mode, hyphen):
        """
        :param format: the output format.
                       One of ``xml``, ``json``, ``tsv`` and ``raw``.
        :param mode: the modus operandi. One of ``sentence`` and ``token``. Both
                     perform sentence splitting; the latter tokenizes the text
                     as well.
        :param hyphen: whether to eliminate word breaks from the end of lines.
        """
        lib.QunToken_new.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool]
        lib.QunToken_new.restype = ctypes.c_void_p
        lib.QunToken_tokenize.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        lib.QunToken_tokenize.restype = ctypes.c_char_p
        lib.QunToken_explain.argtypes = []
        lib.QunToken_explain.restype = ctypes.c_char_p
        self.obj = lib.QunToken_new(
            self.__to_bytes(format), self.__to_bytes(mode), hyphen)
        if not self.obj:
            raise ValueError(self.__to_str(lib.QunToken_explain()))

    def tokenize(self, input):
        """Tokenizes *input* and returns a string."""
        output = lib.QunToken_tokenize(self.obj, self.__to_bytes(input))
        if output is None:
            raise ValueError(self.__to_str(lib.QunToken_explain()))
        return self.__to_unicode(output)

    @staticmethod
    def __to_bytes(s):
        """Converts *s* to a ``bytes`` object."""
        if isinstance(s, QunToken.str_class):
            s = s.encode('utf-8')
        return s

    @staticmethod
    def __to_unicode(s):
        """Converts *s* to a ``unicode`` object."""
        if not isinstance(s, QunToken.str_class):
            s = s.decode('utf-8')
        return s

    @staticmethod
    def __to_str(s):
        """
        Converts *s* to a ``str`` object (i.e., it is equivalent to
        :func:`__to_bytes` in Python 2 and :func:`__to_unicode` on Python 3).
        """
        if sys.version_info.major >= 3:
            return QunToken.__to_unicode(s)
        else:
            return QunToken.__to_bytes(s)


def main():
    pass


if __name__ == '__main__':
    main()
