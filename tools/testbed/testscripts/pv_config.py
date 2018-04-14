testsuits = {
    'link': {
        'tests':{
            'alink5pps': {'script':'alink_testrun.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin', 'args':'--testname=short5pps'},
            'mesh5pps': {'script':'alink_mesh_testrun.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin', 'args':'--testname=short5pps'}
        },
        'wifissid': 'aos_test_01',
        'wifipass': 'Alios@Embedded'
    },
    'mesh': {
        'tests': {
        'line_topology': {'script':'line_topology_v4_test.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin'},
        'tree_topology': {'script': 'tree_topology_v4_test.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin'},
        'multicast': {'script': 'multicast_v4_test.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin'}
        },
        'wifissid': 'alibaba-test-1227981',
        'wifipass': 'Alios@Things'
    },
    'net': {
        'tests': {
        'domain': {'script': 'network_v4_test.py', 'firmware_prefix':'networkapp@', 'firmware_suffix': 'l432kc-nucleo-general.bin'}
        },
        'wifissid': 'alibaba-test-1227982',
        'wifipass': 'Alios@Things'
    },
    'auth': {
        'tests': {
        'meshauth': {'script': 'auth_v4_test', 'firmware_prefix':'meshapp@', 'firmware_suffix': '-meshauth.bin'}
        }
    },
    'ut': {
        'ut': {'script': 'aos_ut.sh', 'firmware_prefix': 'yts@', 'firmware_suffix':'.elf'}
    }
}

models = {
    'linuxhost': [
        ['ut', 'ut']
        ],
    'mk3060': [
        ['link', 'alink5pps'],
        ['link', 'mesh5pps'],
        ['auth', 'meshauth'],
        ['mesh', 'line_topology'],
        ['mesh', 'tree_topology'],
        ['mesh', 'multicast']
        ],
    'esp32': [
        ['link', 'alink5pps'],
        ['auth', 'meshauth'],
        ['mesh', 'line_topology'],
        ['mesh', 'tree_topology'],
        ['mesh', 'multicast']
        ],
    'stm32': [
        ['net', 'domain']
        ]
}

