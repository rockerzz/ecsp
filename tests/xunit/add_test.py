#!/usr/bin/env python

from __future__ import with_statement

import os
import sys
import re
import fileinput

_NAME = "add_test.py"
_VERSION = "0.1"

#global

suitesdir = ""
suitename = ""
suitefilelist = "CMakeLists.txt"
rootdir = "./"

def generateSourceFile():

    global suitename

    pre = ( '#include <gtest/gtest.h>\n\n'
            'namespace ECSP { namespace test {\n\n'
            'class %s: public testing::Test\n'
            '{\n'
            'protected:\n'
            '    static void SetUpTestCase()\n'
            '    {\n\n'
            '    }\n'
            '    static void TearDownTestCase()\n'
            '    {\n\n'   
            '    }\n'
            '    virtual void SetUp()\n'
            '    {\n\n'
            '    }\n'
            '    virtual void TearDown()\n'
            '    {\n\n'
            '    }\n'
            '};\n\n'
            'TEST_F(%s, test1)\n'
            '{\n\n'
            '}\n\n'
            'TEST_F(%s, test2)\n'
            '{\n\n'
            '}\n\n'
            '}} //namespace\n'
          ) % (suitename, suitename, suitename)

    return pre

def main(argv):
    
    global suitename
    global suitesdir
     
    if len(argv) < 3 or not argv[1] or not argv[2]:
        print >> sys.stderr, 'Suite name and suite path expected'
        sys.exit(1)

    suitename = argv[1]
    suitesdir = argv[2]
    
    print >> sys.stdout, 'Suitename: %s' % suitename

    source_file = suitename + '.cc'
    
    dirname = rootdir + suitesdir + '/'

    if not os.path.isdir(dirname):
        print >> sys.stdout, 'create suite dir: %s' % suitename
        os.mkdir(dirname)

    common_dir = dirname + 'common'
    if not os.path.isdir(common_dir):
        os.mkdir(common_dir)

    print >> sys.stderr, '... creating source file "%s"' % source_file
    source_file_text = generateSourceFile()
    source_fp = open(dirname + source_file, 'w')
    print >> source_fp, source_file_text
    source_fp.close()

if __name__ == '__main__':
    main(sys.argv)
