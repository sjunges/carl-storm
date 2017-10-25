#!/usr/bin/env python3

from jinja2 import *

def datamerge(a, b):
	if isinstance(a, list) and isinstance(b, list):
		return a + b
	elif isinstance(a, dict) and isinstance(b, dict):
		res = a.copy()
		for k,v in b.items():
			if k in res:
				res[k] = datamerge(res[k], v)
			else:
				res[k] = v
		return res
	else:
		return b

def property_apt(sources, packages):
	return {
		"sources": ["*sources_base", *sources],
		"packages": ["*packages_base", *packages]
	}

def addons(apt = None, coverity = False, sonarcloud = False):
	res = {}
	if coverity:
		apt = ([],[])
		res["coverity_scan"] = {}
		res["coverity_scan"]["name"] = "smtrat/carl"
		res["coverity_scan"]["description"] = "CArL"
		res["coverity_scan"]["properties"] = {
			"notification_email": "gereon.kremer@cs.rwth-aachen.de",
			"build_command_prepend": "cov-configure --comptype gcc --compiler `which $CXX`",
			"build_command": ".ci/build.sh MAKE_PARALLEL=-j1",
			"branch_pattern": "master",
		}
	if sonarcloud:
		apt = ([],[])
		res["sonarcloud"] = {
			"organization": "smtrat-github",
			"token": "ulhpzLkttWvU5ZydOoQBDF5h/YcExXn6afnAwRlCeqINXxrX6UnzoDImXzfK6i12phdctaSOtZkHy0QuXOZhnRLx0WcTVOOMS5cIzPzUwJBFRD9FaeEHjjQAlCSNhpHcbC9fUfGf7Sh21kNyzMmrGnpCFDMKANECI5vfSf6drpSgbcbckUv6WgJW4kggykXLRGIIEpeA6IvPma+3mC3Wou9O3yWjitBNmP9J+cB6/Dvekbrz6dZttZ58bO5rxKYtkMDjqI0zrmG4hSuILiqTXWG08mHXG4e4N7s43vO0ESXUYq3xWEaZcWgPV5OWXbqpUKIeyaJEpyKunE8bKTiUtpR1uy24oo/8203PB2M/sN3+hHMayPWsoCidBxg4J4ebEaLeHfdAtNccEt3pmNv5JyF4wBY2lY3JhjlxcVF4dW4jP/6sQWM4rbkWJ5quY9U1HnohTxdtzkkKt3/D5y+qcXqdAwPdP7vwMafbrDzf6S4JbBhqjM/kcSn+iJ0CEFsP3InkkAF1KRTO+0fswsh2dbM/jk0Qkq71mTOSwm3jqbqyORLEZGtiJT9Ub6gMZLVR3LBjxTCgfFo7X3nsh4SXTJFhQhsl9XI27V0zo4pz3p7Tkzla4rP4Sl6dYlksIcMX5JGvcPcpsh3UtRt0xqbhV86yYXcd+22zHbUEMSNQwSI="
		}
	if apt != None:
		res["apt"] = property_apt(apt[0], apt[1])
	return res

properties = {
	"dependencies": {"stage": "dependencies", "script": ["TASK=dependencies"]},
	"build": {"stage": "build"},
	
	"xcode7.3": {"os": "osx", "osx_image": "xcode7.3", "env": []},
	"xcode8.2": {"os": "osx", "osx_image": "xcode8.2", "env": []},
	"xcode8.3": {"os": "osx", "osx_image": "xcode8.3", "env": []},
	"linux": {"os": "linux"},
	
	"clang-3.6": {"env": ["CC=clang-3.6 CXX=clang++-3.6"], "compiler": "clang++-3.6", "addons": addons(apt = (["llvm-toolchain-precise-3.6"], ["clang-3.6"]))},
	"clang-3.7": {"env": ["CC=clang-3.7 CXX=clang++-3.7"], "compiler": "clang++-3.7", "addons": addons(apt = (["llvm-toolchain-precise-3.7"], ["clang-3.7"]))},
	"clang-3.8": {"env": ["CC=clang-3.8 CXX=clang++-3.8"], "compiler": "clang++-3.8", "addons": addons(apt = (["llvm-toolchain-precise-3.8"], ["clang-3.8"]))},
	"clang-3.9": {"env": ["CC=clang-3.9 CXX=clang++-3.9"], "compiler": "clang++-3.9", "addons": addons(apt = (["llvm-toolchain-trusty-3.9"], ["clang-3.9"]))},
	"clang-4.0": {"env": ["CC=clang-4.0 CXX=clang++-4.0"], "compiler": "clang++-4.0", "addons": addons(apt = (["llvm-toolchain-trusty-4.0"], ["clang-4.0"]))},
	"clang-5.0": {"env": ["CC=clang-5.0 CXX=clang++-5.0"], "compiler": "clang++-5.0", "addons": addons(apt = (["llvm-toolchain-trusty-5.0"], ["clang-5.0"]))},
	"clang-6.0": {"env": ["CC=clang-6.0 CXX=clang++-6.0"], "compiler": "clang++-6.0", "addons": addons(apt = (["llvm-toolchain-trusty"], ["clang-6.0"]))},
	"g++-5": {"env": ["CC=gcc-5 CXX=g++-5"], "compiler": "g++-5", "addons": addons(apt = ([],["g++-5"]))},
	"g++-6": {"env": ["CC=gcc-6 CXX=g++-6"], "compiler": "g++-6"},
	"g++-7": {"env": ["CC=gcc-7 CXX=g++-7"], "compiler": "g++-7", "addons": addons(apt = ([],["g++-7"]))},
	
	"task.coverage": {"env": ["TASK=coverage"], "addons": addons(apt = ([],["lcov"]))},
	"task.coverity": {"env": ["TASK=coverity"]},
	"task.sonarcloud": {"env": ["TASK=sonarcloud"]},
	"task.doxygen": {"env": ["TASK=doxygen"], "addons": addons(apt = ([],["doxygen", "texinfo", "texlive"]))},
	"task.pycarl": {"env": ["TASK=pycarl"], "addons": addons(apt = ([],["python3"]))},
	"task.addons": {"env": ["TASK=addons"]},
	"task.tidy": {"env": ["TASK=tidy"]},
	
	"addon.coverity": {"addons": addons(coverity = True), "script": ["cat cov-int/scm_log.txt"]},
	"addon.sonarcloud": {"addons": addons(sonarcloud = True)},
	
	"j1": {"script": ["MAKE_PARALLEL=-j1"]},
	"build.sh": {"script": ["source .ci/build.sh"]},
	"mayfail": {"allow_failure": True},
}

def propertymapper(j):
	if "properties" not in j: return j
	res = j.copy()
	for p in j["properties"]:
		res = datamerge(res, properties[p])
	return res

def job(priority, properties):
	return {
		"priority": priority,
		"properties": properties,
	}

jobs = [
	job("0-clang", ["build", "linux", "clang-3.6", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-3.7", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-3.8", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-3.9", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-4.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-5.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-6.0", "build.sh"]),
	job("1-gcc", ["dependencies", "linux", "g++-5", "j1", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-5", "j1", "build.sh"]),
	job("1-gcc", ["dependencies", "linux", "g++-6", "j1", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-6", "j1", "build.sh"]),
	job("1-gcc", ["dependencies", "linux", "g++-7", "j1", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-7", "j1", "build.sh"]),
	job("2-macos", ["build", "xcode7.3", "build.sh"]),
	job("2-macos", ["build", "xcode8.2", "build.sh"]),
	job("2-macos", ["build", "xcode8.3", "build.sh"]),
	job("3-docs", ["build", "linux", "g++-6", "task.doxygen", "j1", "build.sh"]),
	job("4-tidy", ["build", "linux", "clang-5.0", "task.tidy", "build.sh", "mayfail"]),
	job("5-checker", ["dependencies", "linux", "g++-6", "task.coverage", "j1", "build.sh"]),
	job("5-checker", ["build", "linux", "g++-6", "task.coverage", "j1", "build.sh", "mayfail"]),
	job("5-checker", ["dependencies", "linux", "g++-6", "task.coverity", "j1", "build.sh"]),
	job("5-checker", ["build", "linux", "g++-6", "task.coverity", "addon.coverity", "mayfail"]),
	job("5-checker", ["dependencies", "linux", "g++-6", "task.sonarcloud", "j1", "build.sh"]),
	job("5-checker", ["build", "linux", "g++-6", "task.sonarcloud", "addon.sonarcloud", "j1", "build.sh", "mayfail"]),
	job("6-addons", ["dependencies", "linux", "g++-6", "task.pycarl", "j1", "build.sh"]),
	job("6-addons", ["build", "linux", "g++-6", "task.pycarl", "j1", "build.sh", "mayfail"]),
	job("6-addons", ["dependencies", "linux", "g++-6", "task.addons", "j1", "build.sh"]),
	job("6-addons", ["build", "linux", "g++-6", "task.addons", "j1", "build.sh", "mayfail"]),
]

jobs = map(propertymapper, jobs)

jobs = sorted(jobs, key = lambda x: (-len(x["stage"]),x["priority"]))

j2_env = Environment(loader = FileSystemLoader("."), keep_trailing_newline = True)
tpl = j2_env.get_template("travis.yml.tpl")
res = tpl.render(jobs = jobs)

open("../.travis.yml", "w").write(res)
