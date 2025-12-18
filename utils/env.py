import os
import platform


class Env(object):

    @property
    def os(self):
        system = platform.system().lower()
        if system == 'windows':
            if 'armv8' in platform.processor().lower():
                return 'winARM'
            else:
                return 'windows'
        elif system == 'darwin':
            if platform.processor().lower() == 'arm':
                return 'armv8mac'
            else:
                return 'mac-x86-64'
        os_str = os.getenv('OS')
        if os_str is not None:
            return os_str
        if system == 'linux':
            if platform.processor().lower() == 'x86_64':
                return 'i80386linux'
            elif platform.processor().lower() == 'aarch64':
                return 'armv8linux'
        elif system == 'aix':
            return 'rs6000aix'
        elif system == 'sunos':
            if platform.processor().lower() == 'sparc':
                return 'sparcsolaris'
            else:
                return 'intelsolaris'
        elif system == 'hp-ux':
            return 'itanium2hpux'

    def install_folder(self, build_type=None, build_64_bit=None):
        os_str = self.os
        if os_str == 'windows':
            folder = 'win' if build_64_bit else 'win32'
        elif os_str == 'winARM':
            folder = 'winARM'
        elif os_str == 'i80386linux':
            folder = 'linux' if build_64_bit else 'linux32'
        elif os_str == 'sparcsolaris':
            folder = 'sparc' if build_64_bit else 'sparc32'
        elif os_str == 'rs6000aix':
            folder = 'aix' if build_64_bit else 'aix32'
        elif os_str == 'macos':
            folder = 'mac'
        elif os_str == 'armv8mac':
            folder = 'armv8mac'
        else:
            folder = os_str
        suffix = '_debug' if build_type == 'Debug' else '_build'
        return folder + suffix
