project(
  'quasar',
  'c',
  default_options : [
    'c_std=c99',
    'default_library=static',
    'buildtype=release',
    'optimization=3'
  ],
  license : 'AGPL3',
  version : '0.5'
)
bundle_name = 'quasar.lv2'

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)
lv2_dep = dependency('lv2')

# Figure out install_dir
lv2dir = get_option('lv2dir')
if lv2dir == ''
  prefix = get_option('prefix')
  lv2dir = prefix / get_option('libdir') / 'lv2'
endif
bundle_install_dir = lv2dir / bundle_name

conf_data = configuration_data()

lib = shared_library(
  'quasar',
  'quasar.c',
  dependencies : [lv2_dep, m_dep],
  gnu_symbol_visibility : 'hidden',
  install: true,
  install_dir: bundle_install_dir,
  name_prefix : ''
)

conf_data.set('CMAKE_SHARED_LIBRARY_SUFFIX', '.' + lib.full_path().split('.')[-1])

configure_file(
  input : 'quasar.ttl',
  output : 'quasar.ttl',
  copy : true,
  install : true,
  install_dir : bundle_install_dir
)

configure_file(
  input : 'manifest.ttl.in',
  output : 'manifest.ttl',
  configuration : conf_data,
  install : true,
  install_dir : bundle_install_dir
)
