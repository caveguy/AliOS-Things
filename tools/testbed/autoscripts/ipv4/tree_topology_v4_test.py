import sys, os, time
sys.path.append('../../')
from autotest import Autotest

ap_ssid = 'aos_test_01'
ap_pass = 'Alios@Embedded'

devices = {}
devices['A'] = 'mxchip-DN02QRIQ'
devices['B'] = 'mxchip-DN02QRIU'
devices['C'] = 'mxchip-DN02QRIX'
devices['D'] = 'mxchip-DN02QRJ3'
devices['E'] = 'mxchip-DN02QRJ6'
devices['F'] = 'mxchip-DN02QRJ7'
devices['G'] = 'mxchip-DN02QRJ8'

device_list = list(devices)
device_list.sort()
device_attr={}
at=Autotest()
logname=time.strftime('%Y-%m-%d@%H-%M')
logname = 'tree_topology-' + logname +'.log'
if at.start('10.125.52.132', 34568, logname) == False:
    print 'error: start failed'
    exit(1)
if at.device_subscribe(devices) == False:
    print 'error: subscribe to device failed, some devices may not exist in testbed'
    exit(1)

#reboot and get device mac address
retry = 5
for device in device_list:
    succeed = False
    for i in range(retry):
        at.device_control(device, 'reset')
        time.sleep(2.5)
        at.device_run_cmd(device, ['netmgr', 'clear'])
        at.device_run_cmd(device, ['kv', 'del', 'alink'])
        mac =  at.device_run_cmd(device, ['reboot'], 1, 1.5, ['mac'])
        if mac != False and mac != []:
            mac = mac[0].replace('mac ', '')
            mac = mac.replace(':', '')
            mac = mac.replace(' ', '0')
            device_attr[device] = {'mac':mac}
            succeed = True
            break;
    if succeed == False:
        print 'error: reboot and get mac addr for device {0} failed'.format(device)
        exit(1)
time.sleep(5)

bytes = os.urandom(6)
extnetid = ''
for byte in bytes:
    extnetid = extnetid + '{0:02x}'.format(ord(byte))
for device in device_list:
    at.device_run_cmd(device, ['umesh', 'extnetid', extnetid])
    at.device_run_cmd(device, ['umesh', 'stop'])

#setup tree topology
print 'topology:'
print '       A'
print '    /     \\'
print '   B       C'
print ' /   \   /   \\'
print 'D     E F     G\n'
device = 'A'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['B']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['C']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])
device = 'B'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['A']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['D']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['E']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])
device = 'C'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['A']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['F']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['G']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])
device = 'D'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['B']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])
device = 'E'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['B']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])
device = 'F'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['C']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])
device = 'G'
at.device_run_cmd(device, ['umesh', 'whitelist', 'clear'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'add', device_attr['C']['mac']+'0000'])
at.device_run_cmd(device, ['umesh', 'whitelist', 'enable'])
at.device_run_cmd(device, ['umesh', 'whitelist'])

#start devices
filter = ['disabled', 'detached', 'attached', 'leaf', 'router', 'super_router', 'leader', 'unknown']
for i in range(len(device_list)):
    device = device_list[i]
    if i == 0:
        at.device_run_cmd(device, ['netmgr', 'connect', ap_ssid, ap_pass])
        time.sleep(12)
        uuid = at.device_run_cmd(device, ['uuid'], 1, 1)
        if uuid == []:
            print 'error: alink connect to server failed'
            exit(1)
    else:
        at.device_run_cmd(device, ['umesh', 'start'])
        time.sleep(5)

    if i == 0:
        expected_state = 'leader'
    else:
        expected_state = 'router'

    succeed = False; retry = 5
    while retry > 0:
        state = at.device_run_cmd(device, ['umesh', 'state'], 1, 1.5, filter)
        if state == [expected_state]:
            succeed = True
            break
        at.device_run_cmd(device, ['umesh', 'stop'], 1, 1)
        at.device_run_cmd(device, ['umesh', 'start'], 5, 1)
        time.sleep(5)
        retry -= 1
    if succeed == True:
        print '{0} connect to mesh as {1} succeed'.format(device, expected_state)
    else:
        print 'error: {0} connect to mesh as {1} failed'.format(device, expected_state)
        exit(1)

    succeed = False; retry = 3
    while retry > 0:
        ipaddr = at.device_run_cmd(device, ['umesh', 'ipaddr'], 2, 1, ['.'])
        if ipaddr == False or ipaddr == [] or len(ipaddr) != 2:
            continue
        ipaddr[0] = ipaddr[0].replace('\t', '')
        ipaddr[1] = ipaddr[1].replace('\t', '')
        device_attr[device]['ipaddr'] = ipaddr[0:2]
        succeed = True
        break;
    if succeed == False:
        print 'error: get ipaddr for device {0} failed'.format(device)
        exit(1)
for device in device_list:
    print "{0}:{1}".format(device, device_attr[device])

retry = 30
#ping
print 'test connectivity with icmp:'
success_num = 0; fail_num = 0
for device in device_list:
    for other in device_list:
        if device == other:
            continue
        for pkt_len in ['20', '500', '1000']:
            filter = ['bytes from']
            dst_ip = device_attr[other]['ipaddr'][0]
            for i in range(retry):
                response = at.device_run_cmd(device, ['umesh', 'ping', dst_ip, pkt_len], 1, 1.5, filter)
                expected_response = '{0} bytes from {1}'.format(pkt_len, dst_ip)
                if response == False or response == [] or expected_response not in response[0]:
                    if i < retry - 1:
                        continue
                    else:
                        print '{0} ping {1} with {2} bytes by local ip addr failed'.format(device, other, pkt_len)
                        fail_num += 1
                        break
                else:
                    success_num += 1
                    break

print 'ping: succeed-{0}, failed-{1}'.format(success_num, fail_num)

#udp
print '\ntest connectivity with udp:'
success_num = 0; fail_num = 0
for device in device_list:
    for other in device_list:
        if device == other:
            continue
        for pkt_len in ['20', '500', '1000']:
            dst_ip = device_attr[other]['ipaddr'][0]
            filter = ['bytes autotest echo reply from']
            for i in range(retry):
                response = at.device_run_cmd(device, ['umesh', 'autotest', dst_ip, '1', pkt_len], 1, 1, filter)
                expected_response = '{0} bytes autotest echo reply from {1}'.format(pkt_len, dst_ip)
                if response == False or response == [] or expected_response not in response[0]:
                    if i < retry - 1:
                        continue
                    else:
                        print '{0} send {1} with {2} bytes by local ip addr failed'.format(device, other, pkt_len)
                        fail_num += 1
                        break
                else:
                    success_num += 1
                    break

print 'udp: succeed-{0}, failed-{1}'.format(success_num, fail_num)

at.stop()

