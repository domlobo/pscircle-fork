project(
	'pscircle',
	'c',
	default_options : [
		'buildtype=release'
	]
)

config = configuration_data()
config.set('version', '1.0.0')
config.set('NDEBUG', not get_option('buildtype').startswith('debug'))

psc_main = [
	'src/pscircle.c'
]

psc_sources = [
	'src/color.c',
	'src/ppoint.c',
	'src/node.c',
	'src/painter.c',
	'src/ptree.c',
	'src/cfg.c',
	'src/visualizer.c'
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
	'-march=native',
	'-mavx',
	'-ffast-math'
]

cflags = []

foreach flag : test_flags
	if cc.has_argument(flag)
		cflags += flag
	endif
endforeach

psc_library = static_library(
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

if get_option('buildtype').startswith('debug')
	subdir('tests')
endif
