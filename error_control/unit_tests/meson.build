#
# TODO Could split out only those source files needed to support each test. See
# golay test for example.

incdirUT = include_directories(
    '../include/',
    '../../mac/include',
    '../../mac/include/pdu',
    '../third_party/viterbi'
)

unit_test_golay = executable('unit_test-golay', 'qa_golay.cpp', '../src/golay.cpp',
    include_directories : incdirUT,
    dependencies: [gtest_dep]
    )
    
test('golay', unit_test_golay,
    timeout: 30
    )
   
unit_test_no_FEC = executable('unit_test-no_FEC', 'qa_NoFEC.cpp', core_source_files, third_party_source_files,
    include_directories : incdirUT,
    dependencies: [gtest_dep]
    )
    
test('noFEC', unit_test_no_FEC,
    timeout: 30
    )

unit_test_viterbi = executable('unit_test-viterbi', 'qa_viterbi.cpp', core_source_files, third_party_source_files,
    include_directories : incdirUT,
    dependencies: [gtest_dep]
    )
    
test('viterbi', unit_test_viterbi,
    timeout: 100
    )

unit_test_cc_hd_FEC = executable('unit_test-cc_hd_FEC', 'qa_ConvolutionalCodecHD.cpp', core_source_files, third_party_source_files,
    include_directories : incdirUT,
    dependencies: [gtest_dep]
    )
    
test('cc_hd_FEC', unit_test_cc_hd_FEC,
    timeout: 100
    )

