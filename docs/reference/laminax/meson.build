ignore = [
    'Laminax-recorder-src.h',
    'Laminax-recorder.h',
    st_headers,
    st_private_headers,
    tray_headers,
    sniffer_headers,
]

if not internal_nm_agent
    ignore += 'Laminax-network-agent.h'
endif

if not get_option('build_recorder')
    ignore += 'Laminax-recorder.h'
endif

gnome.gtkdoc(
    'Laminax',
    mode: 'xml',
    main_xml: 'Laminax-docs.sgml',
    gobject_typesfile: 'Laminax.types',
    dependencies: [libLaminax_deps, st_dep, Laminax_dep],
    src_dir: [
        join_paths(meson.project_source_root(), 'src'),
        join_paths(meson.project_build_root(), 'src'),
    ],
    scan_args: '--rebuild-types',
    mkdb_args: ['--xml-mode', '--output-format=xml'],
    ignore_headers: ignore,
    install: true,
)
