#!/usr/bin/env python
# coding: utf-8

import io
import sys
import os
import codecs
import re
import json
import argparse
import xml.dom.minidom as minidom

jsTemplate = """require.scopes["%s"] = (function() {
  let exports = {};
%s
  return exports;
})();"""

class CStringArray:
    def __init__(self):
        self._buffer = []
        self._strings = []

    def add(self, string):
        string = string.encode('utf-8').replace('\r', '')
        self._strings.append('std::string(buffer + %i, %i)' % (len(self._buffer), len(string)))
        self._buffer.extend(map(lambda c: str(ord(c)), string))

    def write(self, outHandle, arrayName):
        print >>outHandle, '#include <string>'
        print >>outHandle, 'namespace'
        print >>outHandle, '{'
        print >>outHandle, '  const char buffer[] = {%s};' % ', '.join(self._buffer)
        print >>outHandle, '}'
        print >>outHandle, 'std::string %s[] = {%s, std::string()};' % (arrayName, ', '.join(self._strings))


def addFilesVerbatim(array, files):
    for file in files:
        fileHandle = codecs.open(file, 'rb', encoding='utf-8')
        array.add(os.path.basename(file))
        array.add(fileHandle.read())
        fileHandle.close()


def convertXMLFile(array, file):
    fileHandle = codecs.open(file, 'rb', encoding='utf-8')
    doc = minidom.parse(file)
    fileHandle.close()

    data = []
    for node in doc.documentElement.childNodes:
        if node.nodeType != node.ELEMENT_NODE:
            continue
        result = {'type': node.tagName}
        for name, value in node.attributes.items():
            result[name] = value
        data.append(result)
        fileName = os.path.basename(file)
    array.add(fileName)
    array.add('require.scopes["%s"] = %s;' % (fileName, json.dumps(data)))
    fileHandle.close()


def convertJsFile(array, file):
    with io.open(file, encoding="utf-8") as jsFile:
      jsFileContent = jsFile.read()
    referenceFileName = os.path.basename(file)
    array.add(referenceFileName)
    array.add(jsTemplate % (re.sub("\\.jsm?$", "", referenceFileName), jsFileContent))


def convert(verbatimBefore, convertFiles, verbatimAfter, outFile):
    array = CStringArray()
    addFilesVerbatim(array, verbatimBefore)

    for file in convertFiles:
        if file.endswith('.xml'):
            convertXMLFile(array, file)
        else:
            convertJsFile(array, file)

    addFilesVerbatim(array, verbatimAfter)

    outHandle = open(outFile, 'wb')
    array.write(outHandle, 'jsSources')
    outHandle.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert JavaScript files')
    parser.add_argument('--before', metavar='verbatim_file', nargs='+',
                        help='JavaScript file to include verbatim at the beginning')
    parser.add_argument('--convert', metavar='file_to_convert', nargs='+',
                        help='JavaScript files to convert')
    parser.add_argument('--after', metavar='verbatim_file', nargs='+',
                        help='JavaScript file to include verbatim at the end')
    parser.add_argument('output_file',
                        help='output from the conversion')
    args = parser.parse_args()
    convert(args.before, args.convert, args.after, args.output_file)
