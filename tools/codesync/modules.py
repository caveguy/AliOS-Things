import sys
import traceback
import subprocess
import os
import errno
import argparse
import time

class ProcessException(Exception):
    pass

def popen(command, stdin=None, **kwargs):
    try:
        proc = subprocess.Popen(command, **kwargs)
    except OSError as e:
        if e[0] == errno.ENOENT:
            error(
                "Could not execute \"%s\".\n"
                "Please verify that it's installed and accessible from your current path by executing \"%s\".\n" % (command[0], command[0]), e[0])
        else:
            raise e
    if proc.wait() != 0:
        raise ProcessException(proc.returncode, command[0], ' '.join(list(command)), os.getcwd())

class syncpreparelib():
    def __init__(self, srcbase, dstbase, mkdir):
        mac = ""
        win = ""

        if os.path.exists("./tmp_synccode"):
            linux= "rm -rf tmp_synccode"
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

        linux= "mkdir tmp_synccode"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())
        cur_dir = os.getcwd()
        os.chdir("./tmp_synccode")

        git_cmd = "git clone git@code.aliyun.com:keepwalking.zeng/aos.git"
        popen(git_cmd, shell=True, cwd=os.getcwd())

        os.chdir("./aos")
        if srcbase == "master":
            git_cmd = "git checkout master"
        elif srcbase == "1.0.1":
            git_cmd = "git checkout aos1.0.1"
        else:
            error('Unknown source base!')

        popen(git_cmd, shell=True, cwd=os.getcwd())
        self.srcdir = "./aos"

        linux = "aos make clean"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "aos make meshapp@linuxhost"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "aos make alinkapp@linuxhost"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "aos make alinkapp@mk3060"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        os.chdir(cur_dir)
        os.chdir("./tmp_synccode")
        ######################################
        # add supported boards branch here
        ######################################
        if dstbase == "github":
            git_cmd = "git clone git@github.com:alibaba/AliOS-Things.git"
            popen(git_cmd, shell=True, cwd=os.getcwd())
            self.dstdir = "./AliOS-Things"
        elif dstbase == "mxchip":
            git_cmd = "git clone git@code.aliyun.com:keepwalking.zeng/aos-pbase.git"
            popen(git_cmd, shell=True, cwd=os.getcwd())
            self.dstdir = "./aos-pbase"
        elif dstbase == "allwinner":
            git_cmd = "git clone git@code.aliyun.com:keepwalking.zeng/yunos-iot-project.git"
            popen(git_cmd, shell=True, cwd=os.getcwd())
            self.dstdir = "./yunos-iot-project"
            os.chdir(self.dstdir)
            popen(git_cmd, shell=True, cwd=os.getcwd())
        else:
            error('Unknown source base!')

        self.dstbase = dstbase
        self.srcbase = srcbase
        os.chdir(cur_dir)
        os.chdir("./tmp_synccode")

        self.mkdir = mkdir

    def cleanup_codebase(self):
        mac = ""
        win = ""
        ##############################################################
        # keep platform and board folder the same as different targets
        ##############################################################
        if self.dstbase == "allwinner":
            src = self.dstdir + "/platform"
            linux = "cp -rf " + src + " ./"
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            src = self.dstdir + "/board"
            linux = "cp -rf " + src + " ./"
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            src = self.dstdir + "/example"
            linux = "cp -rf " + src + " ./"
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/*"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/.gitignore"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            src = self.srcdir + "/*"
            dst = self.dstdir
            linux = "cp -rf " + src + " " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/platform"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/board"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/example"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            linux = "cp -rf " + "./platform "+ self.dstdir
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            linux = "cp -rf " + "./board "+ self.dstdir
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            linux = "cp -rf " + "./example "+ self.dstdir
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())
        else:
            dst = self.dstdir + "/*"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/.gitignore"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/.vscode"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            src = self.srcdir + "/*"
            dst = self.dstdir
            linux = "cp -rf " + src + " " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/.gitignore"
        dst = self.dstdir
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/.vscode"
        if os.path.exists(dst):
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            src = self.srcdir + "/.vscode"
            dst = self.dstdir
            linux = "cp -rf " + src + " " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_boards(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/board/armhflinux"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/board/mk108"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_build(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/build/astyle"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/astyle.sh"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/copyright.py"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/copyright"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/doxygen2md.py"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/MD.templet"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/OpenOCD"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/compiler/arm-none-eabi*"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/build/github_sync.sh"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_script(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/script"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_platform(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/platform/mcu/liux/csp/wifi/"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/platform/arch/linux/swap.*"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_kernel(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/kernel/rhino/test"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_example(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/example/mqttest"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/example/tls"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            dst = self.dstdir + "/example/yts"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_bootloader(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/bootloader"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_test(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/test"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

    def cleanup_tools(self):
        mac = ""
        win = ""

        if self.dstbase == "github":
            dst = self.dstdir + "/tools/*"
            linux = "rm -rf " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

            src = self.srcdir + "/tools/Doxyfile"
            if os.path.exists(src):
                dst = self.dstdir + "/tools"
                linux = "cp -rf " + src + " " + dst
                cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
                if not cmd:
                    error('Unknown system!')
                popen(cmd, shell=True, cwd=os.getcwd())

            src = self.srcdir + "/tools/doxygen.sh"
            if os.path.exists(src):
                dst = self.dstdir + "/tools"
                linux = "cp -rf " + src + " " + dst
                cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
                if not cmd:
                    error('Unknown system!')
                popen(cmd, shell=True, cwd=os.getcwd())

            src = self.srcdir + "/tools/cli"
            dst = self.dstdir + "/tools"
            linux = "cp -rf " + src + " " + dst
            cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
            if not cmd:
                error('Unknown system!')
            popen(cmd, shell=True, cwd=os.getcwd())

class meshlib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        mac = ""
        win = ""

        dst = self.dstdir + "/kernel/protocols/mesh/*"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/protocols/mesh/lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/protocols/mesh/lib/linuxhost"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/protocols/mesh/lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/protocols/mesh/include"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/kernel/protocols/mesh/include/umesh_80211.h"
        dst = self.dstdir + "/kernel/protocols/mesh/include/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/kernel/protocols/mesh/include/umesh.h"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/kernel/protocols/mesh/include/umesh_hal.h"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/kernel/protocols/mesh/include/umesh_types.h"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/mesh.mk"
        dst = self.dstdir + "/kernel/protocols/mesh/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/mesh.a"
        dst = self.dstdir + "/kernel/protocols/mesh/lib/mk3060/libmesh.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/meshapp@linuxhost/libraries/mesh.a"
        dst = self.dstdir + "/kernel/protocols/mesh/lib/linuxhost/libmesh.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class ywsslib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        win = ""
        mac = ""
        dst = self.dstdir + "/framework/ywss/*"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/framework/ywss/lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/framework/ywss/lib/linuxhost"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/framework/ywss/lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/framework/ywss/awss.h"
        dst = self.dstdir + "/framework/ywss/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/framework/ywss/enrollee.h"
        dst = self.dstdir + "/framework/ywss/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/ywss.mk"
        dst = self.dstdir + "/framework/ywss/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/ywss.a"
        dst = self.dstdir + "/framework/ywss/lib/mk3060/libywss.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@linuxhost/libraries/ywss.a"
        dst = self.dstdir + "/framework/ywss/lib/linuxhost/libywss.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class rhinolib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        mac = ""
        win = ""

        dst = self.dstdir + "/kernel/rhino/core/*.c"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/rhino/dload"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/rhino/lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/rhino/lib/linuxhost"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/kernel/rhino/lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/rhino.mk"
        dst = self.dstdir + "/kernel/rhino/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/rhino.a"
        dst = self.dstdir + "/kernel/rhino/lib/mk3060/librhino.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@linuxhost/libraries/rhino.a"
        dst = self.dstdir + "/kernel/rhino/lib/linuxhost/librhino.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class wsflib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        mac = ""
        win = ""

        dst = self.dstdir + "/framework/connectivity/wsf/*.c"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/framework/connectivity/wsf/lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/framework/connectivity/wsf/lib/linuxhost"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/framework/connectivity/wsf/lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/wsf.mk"
        dst = self.dstdir + "/framework/connectivity/wsf/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/wsf.a"
        dst = self.dstdir + "/framework/connectivity/wsf/lib/mk3060/libwsf.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@linuxhost/libraries/wsf.a"
        dst = self.dstdir + "/framework/connectivity/wsf/lib/linuxhost/libwsf.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class msdplib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        mac = ""
        win = ""

        if self.srcbase == "1.0.1":
            base_dstdir = self.dstdir + "/framework/protocol/alink/msdp/"
        else:
            base_dstdir = self.dstdir + "/framework/gateway/msdp/"

        dst = base_dstdir + "/*.c"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib/linuxhost"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/msdp.mk"
        dst = base_dstdir
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/msdp.a"

        if self.srcbase == "1.0.1":
            base_dstdir = self.dstdir + "/framework/protocol/alink/msdp/"
        else:
            base_dstdir = self.dstdir + "/framework/gateway/msdp/"
        dst = base_dstdir + "lib/mk3060/libmsdp.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@linuxhost/libraries/msdp.a"
        dst = base_dstdir + "lib/linuxhost/libmsdp.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class devmgrlib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        mac = ""
        win = ""

        if self.srcbase == "1.0.1":
            base_dstdir = self.dstdir + "/framework/protocol/alink/devmgr/"
        else:
            base_srcdir = self.dstdir + "/framework/gateway/devmgr/"

        dst = base_dstdir + "/*.c"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib/linuxhost"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/devmgr.mk"
        dst = base_dstdir
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/devmgr.a"

        if self.srcbase == "1.0.1":
            base_dstdir = self.dstdir + "/framework/protocol/alink/devmgr/"
        else:
            base_dstdir = self.dstdir + "/framework/gateway/devmgr/"
        dst = base_dstdir + "lib/mk3060/libdevmgr.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@linuxhost/libraries/devmgr.a"
        dst = base_dstdir + "lib/linuxhost/libdevmgr.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class gatewaylib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        mac = ""
        win = ""
        base_dstdir = self.dstdir + "/framework/gateway/"
        dst = base_dstdir + "/*.c"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = base_dstdir + "lib/mk3060"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/gateway.mk"
        dst = base_dstdir
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        mac = ""
        win = ""
        src = self.srcdir + "/out/alinkapp@mk3060/libraries/gateway.a"
        base_dstdir = self.dstdir + "/framework/gateway/"
        dst = base_dstdir + "lib/mk3060/libgateway.a"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class bekenlib:
    def __init__(self, srcdir, dstdir, srcbase, dstbase, mkdir):
        self.srcdir = srcdir
        self.dstdir = dstdir
        self.srcbase = srcbase
        self.dstbase = dstbase
        self.mkdir = mkdir

    def make_folder(self):
        win = ""
        mac = ""

        dst = self.dstdir + "/platform/mcu/beken/*"
        linux = "rm -rf " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include/lwip-2.0.2"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include/app"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include/func"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include/os"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include/driver"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/include/ip"
        linux = "mkdir " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/func/mxchip/lwip-2.0.2/port"
        dst = self.dstdir + "/platform/mcu/beken/include/lwip-2.0.2"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/common"
        dst = self.dstdir + "/platform/mcu/beken/include"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/app/config"
        dst = self.dstdir + "/platform/mcu/beken/include/app"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/func/include"
        dst = self.dstdir + "/platform/mcu/beken/include/func"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/os/include"
        dst = self.dstdir + "/platform/mcu/beken/include/os/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/driver/include"
        dst = self.dstdir + "/platform/mcu/beken/include/driver"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/driver/common"
        dst = self.dstdir + "/platform/mcu/beken/include/driver"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/ip/common"
        dst = self.dstdir + "/platform/mcu/beken/include/ip"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/driver/entry/*.h"
        dst = self.dstdir + "/platform/mcu/beken/include"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/build"
        dst = self.dstdir + "/platform/mcu/beken/linkinfo"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken"
        linux = "find " + dst + " -type f -name '*.c' -exec rm {} +"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/beken7231/beken378/build"
        dst = self.dstdir + "/platform/mcu/beken/linkinfo"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/aos"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/hal"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/encrypt_linux"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/encrypt_osx"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/encrypt_win.exe"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/gen_crc_bin.mk"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -rf " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.mkdir + "/beken.mk"
        dst = self.dstdir + "/platform/mcu/beken/"
        linux = "cp -f " + src + " " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

    def make_lib(self):
        build_tool = self.srcdir + "/build/compiler/arm-none-eabi-5_4-2016q2-20160622/Linux64/bin/"
        mac = ""
        win = ""

        linux = "echo \"create libbeken.a\" > packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@mk3060/libraries/beken.a"
        dst = " ."
        linux = "cp " + src + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@mk3060/libraries/entry.a"
        dst = " ."
        linux = "cp " + src + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/out/alinkapp@mk3060/libraries/hal_init.a"
        dst = " ."
        linux = "cp " + src + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        src = self.srcdir + "/platform/mcu/beken/librwnx/librwnx.a"
        dst = " ."
        linux = "cp " + src + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "echo \"addlib beken.a\" >> packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "echo \"addlib entry.a\" >> packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "echo \"addlib hal_init.a\" >> packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "echo \"addlib librwnx.a\" >> packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "echo \"save\" >> packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "echo \"end\" >> packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = build_tool + "arm-none-eabi-ar -M < packscript"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = build_tool + "arm-none-eabi-strip --strip-debug libbeken.a"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken"
        linux = "mv libbeken.a " + dst
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        dst = self.dstdir + "/platform/mcu/beken/"
        linux = build_tool + "arm-none-eabi-ar d " + dst + "aos_main.o"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = build_tool + "arm-none-eabi-ar d " + dst + "soc_impl.o"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = build_tool + "arm-none-eabi-ar d " + dst + "trace_impl.o"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = build_tool + "arm-none-eabi-ar d " + dst + "mesh_wifi_hal.o"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

        linux = "rm *.a"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

class pushcodelib():
    def __init__(self, basedir, srcdir, dstdir):
        self.basedir = basedir
        self.srcdir = srcdir
        self.dstdir = dstdir

    def execute(self):
        os.chdir(self.dstdir);
        git_cmd = "git checkout -b code_sync_branch"
        popen(git_cmd, shell=True, cwd=os.getcwd())
        git_cmd = "git add -A"
        popen(git_cmd, shell=True, cwd=os.getcwd())
        git_cmd = "code synchronization at " + time.asctime()
        git_cmd = "git commit -m " + "\"" + git_cmd + "\""
        popen(git_cmd, shell=True, cwd=os.getcwd())
        git_cmd = "git push origin code_sync_branch"
        popen(git_cmd, shell=True, cwd=os.getcwd())

    def cleancode(self):
        mac = ""
        win = ""

        os.chdir(self.basedir)
        linux = "rm -rf tmp_synccode"
        cmd = mac if sys.platform == 'darwin' else (linux if sys.platform == 'linux2' else (win if sys.platform == 'win32' else None))
        if not cmd:
            error('Unknown system!')
        popen(cmd, shell=True, cwd=os.getcwd())

