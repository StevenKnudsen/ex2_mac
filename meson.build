project('ex2_obc_sdr', 'c', 'cpp',
    license: 'proprietary',
    version: '0.5.0',
    default_options : [
        'buildtype=release',
        'warning_level=3',
#        'buildtype=debug',
#        'werror=true',
        ],
    )

cpp = meson.get_compiler('cpp')

home = run_command('sh', '-c', 'echo $HOME', check: true)
HOME_DIR = home.stdout().strip()
message('home = ' + HOME_DIR)

prj_dir = run_command('sh', '-c', 'echo $PWD', check: true)
PRJ_DIR = prj_dir.stdout().strip()
message('prj_dir = ' + PRJ_DIR)

#boost_dep = dependency('boost', modules: [
#        'program_options',
#        'iostreams',
#        'filesystem',
#        'system',
#        'thread'
#        ],
#    )

eigen_dep = dependency('eigen3')


core_source_files = [
#    PRJ_DIR / 'error_control/src/crc.cpp',
#    PRJ_DIR / 'error_control/src/interleaver.cpp',
#    PRJ_DIR / 'error_control/src/scrambler.cpp',
    PRJ_DIR / 'error_control/src/error_correction.cpp',
    PRJ_DIR / 'error_control/src/ConvolutionalCodecHD.cpp',
    PRJ_DIR / 'error_control/src/FEC.cpp',
    PRJ_DIR / 'error_control/src/NoFEC.cpp',
    PRJ_DIR / 'error_control/src/QCLDPC.cpp',
    PRJ_DIR / 'error_control/src/golay.cpp',
]

incdir = include_directories(
    'error_control/include',
    'mac/include',
    'mac/include/pdu',
    'error_control/third_party/viterbi'
)

third_party_source_files = [
    PRJ_DIR / 'error_control/third_party/viterbi/viterbi.cpp'
]

gtest_dep = dependency('gtest_main', required: false)

if not gtest_dep.found()
    foreach incdir: ['/usr/include/gtest', '/usr/local/include/gtest']
        if cpp.has_header('gtest.h', args: '-I' + incdir)
            gtest_inc = include_directories(incdir)
            break
        endif
    endforeach
    
    gtest_dep = declare_dependency(
        dependencies: [cpp.find_library('gtest'),cpp.find_library('gtest_main')],
        include_directories: gtest_inc,
        )
endif

if not gtest_dep.found()
    error('unable to find gtest dependency')
endif

#thread_dep = dependency('threads')
if gtest_dep.found()
    subdir('error_control/unit_tests')
    subdir('mac/unit_tests')
endif

#subdir('error_control/doc')
#subdir('mac/doc')
