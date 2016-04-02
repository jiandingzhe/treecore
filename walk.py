# -*- coding: utf-8 -*-

import os

include_file_names = set()

def include(filename) :
    if os.path.exists(filename) :
        if os.path.isdir(filename) :
            list_dirs = os.walk(filename)
            for root, dirs, files in list_dirs:
                for d in dirs:
                    print os.path.join(root, d)
                for f in files:
                    print os.path.join(root, f)
    else:
        print filename

def 添加文件(文件名称) :
    include(文件名称)

添加文件("F:\Spectrasonics")