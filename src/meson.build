subdir('st')
subdir('hotplug-sniffer')

include_src = include_directories('.')

calendar_generated = gnome.gdbus_codegen('Laminax-calendar',
  sources: 'org.Laminax.CalendarServer.xml',
  interface_prefix: 'org.Laminax.',
  namespace: 'Laminax'
)

Laminax_headers = [
    'Laminax-action-mode.h',
    'Laminax-app.h',
    'Laminax-app-system.h',
    'Laminax-doc-system.h',
    'Laminax-embedded-window.h',
    'Laminax-generic-container.h',
    'Laminax-glsl-effect.h',
    'Laminax-gtk-embed.h',
    'Laminax-global.h',
    'Laminax-perf-log.h',
    'Laminax-screen.h',
    'Laminax-screenshot.h',
    'Laminax-slicer.h',
    'Laminax-stack.h',
    'Laminax-touchegg-client.h',
    'Laminax-tray-icon.h',
    'Laminax-tray-manager.h',
    'Laminax-util.h',
    'Laminax-window-tracker.h',
    'Laminax-wm.h',
]

Laminax_private_headers = [
    'Laminax-app-private.h',
    'Laminax-app-system-private.h',
    'Laminax-embedded-window-private.h',
    'Laminax-global-private.h',
    'Laminax-window-tracker-private.h',
    'Laminax-wm-private.h',
]

Laminax_sources = [
    'Laminax-plugin.c',
    'Laminax-app.c',
    'Laminax-app-system.c',
    'Laminax-doc-system.c',
    'Laminax-embedded-window.c',
    'Laminax-generic-container.c',
    'Laminax-glsl-effect.c',
    'Laminax-gtk-embed.c',
    'Laminax-global.c',
    'Laminax-keyring-prompt.c',
    'Laminax-keyring-prompt.h',
    'Laminax-perf-log.c',
    'Laminax-polkit-authentication-agent.c',
    'Laminax-polkit-authentication-agent.h',
    'Laminax-screen.c',
    'Laminax-screenshot.c',
    'Laminax-secure-text-buffer.c',
    'Laminax-secure-text-buffer.h',
    'Laminax-slicer.c',
    'Laminax-stack.c',
    'Laminax-touchegg-client.c',
    'Laminax-tray-icon.c',
    'Laminax-tray-manager.c',
    'Laminax-util.c',
    'Laminax-window-tracker.c',
    'Laminax-wm.c',
    Laminax_headers,
    calendar_generated
]

Laminax_enum_types = gnome.mkenums_simple(
    'Laminax-enum-types',
    sources: Laminax_headers,
)

Laminax_sources += Laminax_enum_types

tray_headers = [
    'tray/na-tray-child.h',
    'tray/na-tray-manager.h',
]

tray_sources = [
    'tray/na-tray-child.c',
    'tray/na-tray-manager.c',
    tray_headers,
]

libLaminax_deps = [
    atk,
    cjs,
    clutter,
    cmenu,
    cogl,
    cogl_path,
    dbus,
    gcr,
    gdkx11,
    gi,
    gio,
    gio_unix,
    gl,
    gtk,
    muffin,
    pango,
    polkit,
    st_dep,
    xapp,
    xml,
]

libLaminax_deps += nm_deps

if internal_nm_agent
    Laminax_sources += [
        'Laminax-network-agent.c',
        'Laminax-network-agent.h',
    ]
endif

non_gir = []
if get_option('build_recorder')
    Laminax_sources += [
        'Laminax-recorder.c',
        'Laminax-recorder.h',
    ]

    non_gir += [
        'Laminax-recorder-src.c',
        'Laminax-recorder-src.h',
    ]

    libLaminax_deps += [
        gstreamer,
        gstreamer_base,
    ]
endif

install_rpath = ':'.join([muffin_typelibdir, pkglibdir])


libLaminax = library(
    'Laminax',
    [Laminax_sources, non_gir, tray_sources, st_enum_types[1]],
    include_directories: [include_root, include_directories('st', 'tray')],
    install_rpath: install_rpath,
    build_rpath: muffin_typelibdir,
    dependencies: libLaminax_deps,
    install: true,
    install_dir: pkglibdir,
)

Laminax_dep = declare_dependency(
    include_directories: include_directories('.'),
    dependencies: libLaminax_deps,
    link_with: libLaminax,
    link_args: ['-Wl,-Bsymbolic', '-Wl,-z,relro', '-Wl,-z,now'],
)

executable(
    'Laminax-perf-helper',
    'Laminax-perf-helper.c',
    include_directories: include_root,
    dependencies: [config_h, gtk],
    install: true,
    install_dir: libexecdir,
    install_rpath: install_rpath,
    build_rpath: muffin_typelibdir
)

# FIXME
# js_tester = executable(
#     'run-js-test',
#     ['run-js-test.c', Laminax_enum_types[1], st_enum_types[1]],
#     c_args: [
#         '-DLaminax_PKGLIBDIR="@0@"'.format(join_paths(prefix, pkglibdir)),
#     ],
#     include_directories: include_root,
#     dependencies: Laminax_dep,
#     install: false,
# )

executable(
    'Laminax',
    ['main.c', Laminax_enum_types[1], st_enum_types[1]],
    c_args: [
        '-DLOCALEDIR="@0@"'.format(join_paths(prefix, datadir, 'locale')),
        '-DLaminax_PKGLIBDIR="@0@"'.format(join_paths(prefix, pkglibdir)),
    ],
    install_rpath: install_rpath,
    build_rpath: muffin_typelibdir,
    include_directories: [include_root, st_inc],
    dependencies: Laminax_dep,
    install: true,
)

Laminax_gir_includes = [
    'Clutter-0',
    'ClutterX11-0',
    'CoglPango-0',
    'Cogl-0',
    'Meta-0',
    'CMenu-3.0',
    'Gcr-3',
    'PolkitAgent-1.0',
    st_gir[0],
]

if internal_nm_agent
    Laminax_gir_includes += ['NM-1.0']
endif

Laminax_gir = gnome.generate_gir(
    libLaminax,
    namespace: 'Laminax',
    nsversion: '0.1',
    sources: Laminax_sources,
    dependencies: libLaminax_deps,
    identifier_prefix: 'Laminax',
    symbol_prefix: 'Laminax',
    export_packages: 'Laminax',
    includes: Laminax_gir_includes,
    install: true,
    install_dir_typelib: pkglibdir,
    install_dir_gir: pkgdatadir,
    extra_args: [
        '-DST_COMPILATION',
        '--quiet',
        '--warn-all',
    ],
)

