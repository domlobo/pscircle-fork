project(
	'pscircle',
	'c'
)

config = configuration_data()
config.set('version', '1.0.0')

psc_main = [
	'src/pscircle.c'
]

psc_sources = [
	'src/color.c'
]

configure_file(
	input : 'config.h.meson',
	output : 'config.h',
	configuration : config
)

incdir = [
	include_directories('.'),
	include_directories('include')
]

cc = meson.get_compiler('c')

deps = [
	dependency('cairo'),
	dependency('libpng'),
	cc.find_library('m', required : false),
]

test_flags = [
#	'-march=native',
#	'-mavx',
#	'-ffast-math'
]

cflags = []

foreach flag : test_flags
	if cc.has_argument(flag)
		cflags += flag
	endif
endforeach

psc_library = library(
	'pscircle',
	sources : psc_sources,
	include_directories : incdir,
	dependencies : deps,
	c_args : cflags
)

executable(
	'pscircle',
	sources : psc_main,
	include_directories : incdir,
	link_with : psc_library,
	dependencies : deps,
	c_args : cflags,
	install: true
)

tests = [
	['test_color', ['tests/test_color.cpp']]
]

if ['debug', 'debugoptimized'].contains(get_option('buildtype'))
	add_languages('cpp')
	gtest_dep = dependency('gtest', main : true, required : true)

	#if gtest_dep.found()
		deps += gtest_dep

		foreach t : tests
			exe = executable(
				t[0],
				t[1],
				include_directories : incdir,
				link_with : psc_library,
				dependencies : deps,
			)

			test(t[0], exe)
		endforeach

	#endif

endif
