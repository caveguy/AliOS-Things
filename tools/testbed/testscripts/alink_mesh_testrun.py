import sys, os, time, httplib, json, subprocess, pdb
from autotest import Autotest

required_devices = [ [1, 'general'], [1, 'alink'] ]

models={'mk3060':'0x13200', 'esp32':'0x10000'}
testnames={'5pps':1, '2pps':0, 'short5pps':2, 'short2pps':3}

caseids={}
#mk3060 alink
caseids['1399F22BC9A731018D4BC6210A83108D'] = 54709 #mk3060-DN02RDVL
caseids['2B7A3D809619DFCFA76A295A104277D7'] = 54713 #mk3060-DN02X30H
caseids['1B4A5AFEB7D3501950D8E5615C3F0D72'] = 54705 #mk3060-DN02X30I

#esp32 mesh alink
caseids['F9BD9E6689F406041EE5FC244E096EF0'] = 54677 #esp32-2.1.1
caseids['06D754F95B2BC4802881CB25322AE35E'] = 54697 #esp32-2.1.2
caseids['4FE2302CF7F04646C2A18172FCC78673'] = 54683 #esp32-2.2.1

DEBUG = False
#server inteaction related functions
operations = {'status':'getCaseStatus', 'start': 'runCase', 'stop':'stopCase'}
statuscode = {'0':'idle', '1':'running', '2':'success', 3:'fail'}
def construct_url(operation, testid, auid):
    if operation not in list(operations):
        return ''
    if testid.isdigit() == False:
        return ''
    if auid.isdigit() == False:
        return ''
    url = '/' + operations[operation] + '?id=' + testid + '&auid=' + auid
    return url


def alink_test(conn, operation, testid, auid):
    headers = {'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
               'Accept-Encoding': 'gzip, deflate',
               'Accept-Language': 'en-us',
               'Connection': 'keep-alive',
               'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/603.3.8 (KHTML, like Gecko) Version/10.1.2 Safari/603.3.8'}
    url = construct_url(operation, testid, auid)
    if url == '':
        return {}

    succeed = False; retry = 3
    while retry > 0:
        try:
            conn.request('GET', url, '', headers)
            succeed = True
            break
        except:
            time.sleep(2)
            retry -= 1
    if succeed == False:
        print 'error: connecting server to request service failed'
        return {}
    response = conn.getresponse()
    if response.status != 200:
        print 'http request error: retcode-{0}'.format(response.status)
        return {}
    respdata = response.read()
    try:
        return json.loads(respdata)
    except:
        if DEBUG:
            raise
        return {}

def restore_extnetid(at, device_list):
    #restore extnetid to default
    extnetid = '010203040506'
    for device in device_list:
        at.device_run_cmd(device, 'umesh extnetid {0}'.format(extnetid))
        at.device_run_cmd(device, 'umesh whitelist disable')

#main function
def main(firmware='~/lb-all.bin', model='mk3060', testname='5pps'):
    global DEBUG
    userid = '500001280426201742'
    alink_test_server = 'pre-iotx-qs.alibaba.com'
    wifissid = 'aos_test_01'
    wifipass = 'Alios@Embedded'
    server = '10.125.52.132'
    port = 34568

    #parse input
    i = 1
    while i < len(sys.argv):
        arg = sys.argv[i]
        if arg.startswith('--firmware='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --firmware=firmware.bin'.format(arg)
            firmware = args[1]
        elif arg.startswith('--model='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --model=mk3060'.format(arg)
            model = args[1]
        elif arg.startswith('--testname='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --testname=5pps'.format(arg)
            testname = args[1]
        elif arg.startswith('--userid='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --userid=123456789012345678'.format(arg)
            userid = args[1]
        elif arg.startswith('--server='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --server=10.1.2.3'.format(arg)
            server = args[1]
        elif arg.startswith('--port='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --port=34568'.format(arg)
            port = int(args[1])
        elif arg.startswith('--wifissid='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifissid=test_wifi'.format(arg)
            wifissid = args[1]
        elif arg.startswith('--wifipass='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifipass=test_password'.format(arg)
            wifipass = args[1]
        elif arg.startswith('--debug='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --debug=1'.format(arg)
            DEBUG = (args[1] != '0')
        elif arg=='--help':
            print 'Usage: python {0} [--firmware=xxx.bin] [--model=xxx] [--testname=xxxx] [--userid=xxxxx] [--server=xx.x.x.x] [--port=xx] [--wifissid=wifi_ssid] [--wifipass=password] [--debug=0/1]'.format(sys.argv[0])
            return [0, 'help']
        i += 1

    if DEBUG:
        print "firmware: {0}".format(firmware)
        print "model: {0}".format(model)
        print "testname: {0}".format(testname)
        print "userid: {0}".format(userid)
        print "server: {0}".format(server)
        print "port: {0}".format(port)
        print "wifissid: {0}".format(wifissid)
        print "wifipass: {0}".format(wifipass)

    #parameter check
    if testname not in testnames:
        print "error: unsupported testname {0}".format(repr(testname))
        return [1, 'testname {0} unsupported'.format(repr(testname))]

    if not model or model.lower() not in models:
        print "error: unsupported model {0}".format(repr(model))
        return [1, 'model {0} unsupported'.format(repr(model))]
    model = model.lower()

    #connect to testbed server
    logname=time.strftime('-%Y-%m-%d@%H-%M')
    logname = testname + logname +'.log'
    at=Autotest()
    if at.start(server, port, logname) == False:
        print 'error: start failed'
        return [1, 'connect testbed failed']

    #request allocating devices
    devices = {}
    timeout = 10
    number, purpose = required_devices[0]
    allocated_leader = at.device_allocate(model, number, timeout, purpose)
    number, purpose = required_devices[1]
    allocated_router = at.device_allocate(model, number, timeout, purpose)
    if len(allocated_leader) != number or len(allocated_router) != number:
        print "error: request allocating devices failed"
        return [1, 'allocate device failed']
    devices['A'] = allocated_leader[0]
    devices['B'] = allocated_router[0]
    print "allocated: leader-{0}, router-{1}".format(allocated_leader[0], allocated_router[0])

    #subscribe device
    if at.device_subscribe(devices) == False:
        print 'error: subscribe to device failed, some devices may not exist in testbed'
        return [1, 'subscribe device failed']
    for device in devices:
        at.device_run_cmd(device, 'help')

    #program device
    succeed = False; retry = 5
    addr = models[model]
    for device in devices:
        print 'programming device {0} ...'.format(devices[device])
        for i in range(retry):
            if at.device_program(device, addr, firmware) == True:
                succeed = True
                break
            time.sleep(0.5)
        if succeed == False:
            print 'error: program device {0} failed'.format(devices[device])
            return [1, 'program device failed']
        print 'program device {0} succeed'.format(devices[device])
    time.sleep(5)

    #generate a random mesh extnetid
    bytes = os.urandom(6)
    extnetid = ''
    for byte in bytes:
        extnetid = extnetid + '{0:02x}'.format(ord(byte))
    for device in devices:
        at.device_control(device, 'reset')
        time.sleep(5)
        at.device_run_cmd(device, 'umesh extnetid {0}'.format(extnetid))

    #connect leader to alink
    device = 'A'
    succeed = False; retry = 5
    uuid = None
    while retry > 0:
        #clear previous setting and reboot
        at.device_run_cmd(device, 'kv del wifi')
        at.device_run_cmd(device, 'kv del alink')
        at.device_control(device, 'reset')
        time.sleep(5)

        #connect device to alink
        at.device_run_cmd(device, 'netmgr connect {0} {1}'.format(wifissid, wifipass), timeout=1.5)
        time.sleep(30)
        filter = ['uuid:', 'alink is not connected']
        role = at.device_run_cmd(device, 'umesh status', 1, 1.5, ['state\t'])
        response = at.device_run_cmd(device, 'uuid', 1, 1.5, filter)
        if role == False or len(role) != 1 or 'leader' not in role[0]:
            retry -= 1
            continue
        if response == False or len(response) != 1 or 'uuid:' not in response[0]:
            retry -= 1
            continue
        uuid = response[0].split()[-1]
        if len(uuid) != 32:
            retry -= 1
            continue
        print "connect mesh leader to alink succeed, uuid: {0}".format(uuid)
        succeed = True
        break;
    if succeed == False:
        print 'error: connect leader device  to alink failed, response = {0}'.format(response)
        restore_extnetid(at, list(devices))
        return [1, 'connect alink failed']

    #connect router to alink
    device = 'B'
    succeed = False; retry = 5
    uuid = None
    while retry > 0:
        #clear previous setting and reboot
        at.device_run_cmd(device, 'kv del wifi')
        at.device_run_cmd(device, 'kv del alink')
        at.device_control(device, 'reset')
        time.sleep(5)

        #connect device to alink
        time.sleep(30)
        filter = ['uuid:', 'alink is not connected']
        role = at.device_run_cmd(device, 'umesh status', 1, 1.5, ['state\t'])
        response = at.device_run_cmd(device, 'uuid', 1, 1.5, filter)
        if role == False or len(role) != 1 or 'router' not in role[0]:
            retry -= 1
            continue
        if response == False or len(response) != 1 or 'uuid:' not in response[0]:
            retry -= 1
            continue
        uuid = response[0].split()[-1]
        if len(uuid) != 32:
            retry -= 1
            continue
        print "connect mesh router to alink succeed, uuid: {0}".format(uuid)
        succeed = True
        break;
    if succeed == False:
        print 'error: connect router device to alink failed, response = {0}'.format(response)
        restore_extnetid(at, list(devices))
        return [1, 'connect alink failed']
    if uuid not in caseids:
        print 'error: device uuid {0} not in supported list'.format(uuid)
        restore_extnetid(at, list(devices))
        return [1, 'uuid {0} invalid'.format(uuid)]
    caseid = caseids[uuid] + testnames[testname]
    caseid = str(caseid)
    print "alink test caseid: {0}".format(caseid)

    #login through gateway
    at.device_run_cmd(device, 'gateway login')
    time.sleep(3)
    at.device_run_cmd(device, 'gateway login')
    time.sleep(3)

    #check test case status
    already_running = False
    conn = httplib.HTTPConnection(alink_test_server, '80')
    result = alink_test(conn, 'status', caseid, userid)
    if DEBUG:
        print 'status:', result
    if result == {} or result[u'message'] != u'success':
        print 'error: unable to get test case {0} status'.format(caseid)
        restore_extnetid(at, list(devices))
        return [1, 'get case {0} status failed'.format(caseid)]
    if result[u'data'][u'case_status'] == 1:
        print 'test case {0} is already runing'.format(caseid)
        already_running = True
    conn.close()

    if already_running:
        #already running, stop test case
        conn = httplib.HTTPConnection(alink_test_server, '80')
        result = alink_test(conn, 'stop', caseid, userid)
        if DEBUG:
            print 'status:', result
        if result == {} or result[u'message'] != u'success':
            print 'error: unable to stop test case {0}'.format(caseid)
            restore_extnetid(at, list(devices))
            return [1, 'stop alink testcase {0} failed'.format(caseid)]
        conn.close()
        print 'stop case {0} succeed'.format(caseid)

    #start run test case
    conn = httplib.HTTPConnection(alink_test_server, '80')
    result = alink_test(conn, 'start', caseid, userid)
    if DEBUG:
        print 'start:', result
    if result == {}:
        print 'error: unable to start test case {0}'.format(caseid)
        restore_extnetid(at, list(devices))
        return [1, 'start case failed']
    if result[u'message'] != u'success':
        print 'error: start test case {0} failed, return:{1}'.format(caseid, result[u'message'])
        restore_extnetid(at, list(devices))
        return [1, 'start alink testcase {0} failed'.format(caseid)]
    conn.close()
    time.sleep(5)

    #poll test case status
    retry = 5
    while retry > 0:
        try:
            conn = httplib.HTTPConnection(alink_test_server, '80')
            result = alink_test(conn, 'status', caseid, userid)
            if DEBUG:
                print 'status:', result
            if result == {}:
                print 'error: unable to get test case {0} status'.format(caseid)
                restore_extnetid(at, list(devices))
                return [1, 'get status failed']
            if result[u'message'] != u'success' or result[u'data'][u'case_status'] != 1:
                break;
            conn.close()
        except:
            retry -= 1
        time.sleep(120)

    #print result
    try:
        print result[u'data'][u'case_fail_desc'].encode('utf-8')
    except:
        pass

    if result[u'data'][u'case_status'] != 2:
        print 'test {0} finished unsuccessfully'.format(testname)
        restore_extnetid(at, list(devices))
        return [1, 'failed']
    else:
        print 'test {0} finished successfully'.format(testname)
        restore_extnetid(at, list(devices))
        return [0, 'passed']


if __name__ == '__main__':
    #flush to output immediately
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
    sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 0)
    [code, msg] = main()
    sys.exit(code)
