
def add_header(path):
	
	if path.contain('{SRC_PATH}') :
		
	elif path.contain('{LIB_PATH}') :

	return

class Project:

	def add_header(path):

		if path.contain('{SRC_PATH}') :

		elif path.contain('{LIB_PATH}') :

		return

	dict mHeaders
	dict mFlags
	dict mMarcos

#------------------------------------

class Project :

	__depence = set()
	__name = ''

	def __init__(self,name) :
		self.__name = name

	def name(self) :
		return self.__name

	def depence(self,*projects) :
		for project in projects :
			for i in project.__depence :
				self.__depence.add(i.name())


Project ztd("zentraedi")
Project tree_core("tree_core")
Project tree_face("tree_face")
tree_face.depence("tree_core")
ztd.depence("tree_face")


solution_name = ""

class nomake:

	__name = ""
	__projects = dict()

	def __init__(self,name) :
		self.__name = name

	def __del__(self) :
		os.file

	def link_solution(self,solutionName,projectName) :


	def addProject(self,projectName):
		self.__projects[projectName]=Project(projectName)

	def getProject(self,projectName):
		return self.__projects[projectName]

	def __getitem__(self, name):   
        return self.__projects[name]


import os
filename = os.environ.get('PYTHONSTARTUP')
if filename and os.path.isfile(filename):
    execfile(filename)


def include(name):
	import os
	import sys
	name,env = name.replace("{$...$}")
	nomakePath = os.environ.get(env)
	fullPath = name + env
	if os.path.isfile(name):
		sys.path.append(fullPath+name)
		import name

{$dsadsa$}

include_file_names = set()

def include(filename) :
	if os.path.isdir(filename) :
		for os

nomake.include("{$ZTD_PATH$}/Zentraedi")

nomake k("Heavier7Strings")

nomake.add_solution("Heavier7Strings")

nomake.add_project("Heavier7Strings","plugin")

nomake.get_project("Heavier7Strings","plugin").set_required( nomake.get_project("Zentraedi","lib") )

nomake.get_project("Heavier7Strings","plugin").set_target("debug_x86","debug_x64","release_x64","release_x86")

nomake.get_project("Heavier7Strings","plugin","debug_x86").add_complier_flag("_DEBUG","DEBUG")

nomake.get_project("Heavier7Strings").add_file("{SRC_PATH}/src/Interlocked/*.h","-_win")

nomake.get_project("Heavier7Strings").add_file("{SRC_PATH}/src/Interlocked/*.cpp","_win")

nomake.gen( "msvc2015",
	        "msvc2015-icc",
	        "codeblock-win",
	        "codeblock-win-icc",
	        "qtcreator-win",
	        "qtcreator-linux",
	        "qtcreator-mac",
	        "xcode",
	        "win-makefile",
	        "linux-makefile" )


#解决方案->项目->目标