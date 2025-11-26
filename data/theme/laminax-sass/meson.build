sassc = find_program('pysassc', 'sassc')

custom_target(
  'Laminax_css',
  input: 'Laminax.scss',
  output: 'Laminax.css',
  command: [sassc, '@INPUT@', '@OUTPUT@'],
  build_always_stale: true,
  install: true,
  install_dir: join_paths(pkgdatadir, 'theme')
)
