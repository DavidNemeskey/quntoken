#! /usr/bin/env python3
# coding=utf8
"""Googletest forrasfajl generalasa (tmp/test.cpp).

A generalashoz meg kell adni a sablon fajlt es a teszteseteket
tartalmazo fajlokat.
"""
# TODO: jelen formában nem alkalmas a sorvégek tesztelésére (csak "\n" lehet)

import sys
import glob
import argparse
from string import Template


def args_handling():
    """ Parancssori argumentumok kezelése.
    """
    # TODO: golb-ot kivaltani majd argumentumban megadott fajlok sorozataval!
    descr_tmpl = 'A templatumot tartalmazo forrasfajlok.'
    descr_data = 'A testeseteket tartalmazo forrasfajlok'
    descr_out  = 'Cel fájl neve (eleresi uttal egyutt).'
    pars = argparse.ArgumentParser(description=__doc__)
    pars.add_argument(
            '-t',
            '--template-file',
            help=descr_tmpl,
            required=True,
            nargs=1)
    pars.add_argument(
            '-d',
            '--data-files',
            help=descr_data,
            required=True,
            nargs='+')
    pars.add_argument(
            '-o',
            '--object-file',
            help=descr_out,
            required=True,
            nargs=1)
    return vars(pars.parse_args())

def process_file(file_):
    res = []
    inp = ''
    out = ''
    flipflop = False
    for line in file_:
        line = line.rstrip()
        inp_line = line.startswith(INP_PREFIX) or line == INP_PREFIX[:-1]
        out_line = line.startswith(OUT_PREFIX) or line == OUT_PREFIX[:-1]
        if not inp_line and not out_line:
            continue
        line = line[len(INP_PREFIX):] # feltesszük, hogy a két prefix hossza
                                      # megegyezik
        line = line.replace('"', '\\"')
        if inp_line:
            if not flipflop:
                if inp or out: # előző kör kiírása
                    res.append(ASSERT_TEMPLATE.substitute(INP=inp, OUT=out))
                flipflop = True
                inp = line
            else:
                inp += '\\n' + line
        elif out_line:
            if flipflop:
                flipflop = False
                out = line
            else:
                out += '\\n' + line
    if inp or out: # utolsó kör kiírása
        res.append(ASSERT_TEMPLATE.substitute(INP=inp, OUT=out))
    return '\n    '.join(res)


def generate_assertion(test_file):
    """Visszater a kapott fajlbol kinyerheto testesthez tartozo koddal.
    """
    with open(test_file) as file_:
        return process_file(file_)


def generate_cpp(tmpl, assertions):
    """Behelyettesiti a template-be az osszegyujtott testeseteket.
    """
    tmpl = Template(tmpl.read())
    return tmpl.substitute(ASSERTIONS=assertions)


def main():
    args = args_handling()
    TMPL_FILE = args['template_file'][0]
    DATA_FILES = args['data_files'] # lista!
    OUT_FILE  = args['object_file'][0]
    
    assertions = [ generate_assertion(x) for x in DATA_FILES ]
    assertions = '\n    '.join(assertions)

    with open(TMPL_FILE, 'r') as tmpl:
        cpp = generate_cpp(tmpl, assertions)

    with open(OUT_FILE, 'w') as out:
        out.write(cpp)


if __name__ == "__main__":
    INPUT_DIR = "test/"
    OUTPUT_DIR = "tmp/"
    INP_PREFIX = "IN : "
    OUT_PREFIX = "OUT: "
    # A output végén mindig lesz egy sortörés, azt pedig itt is meg kell adni.
    ASSERT_TEMPLATE = Template("""
    ASSERT_STREQ(
        "${OUT}\\n",
        process_text("${INP}").c_str());
    """)
    main()

