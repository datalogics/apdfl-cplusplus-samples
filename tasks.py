import os
import shutil
import platform
import re
import fnmatch
from dl_val_toolkit.bootstrap import bootstrap as dl_val_toolkit_bootstrap
from dl_val_toolkit.login import login
from invoke import Collection, task, runners
from invoke.tasks import Task
from utils import env

@task
def distclean(ctx):
    """Clean up the project to its pristine distribution state. Undoes the effects of bootstrap."""
    for file in os.listdir('.'):
        if fnmatch.fnmatch(file, '*_build*'):
            print('Removing ', file)
            shutil.rmtree(file)


def noerr_mkdir(dirname):
    try:
        os.makedirs(dirname)
    except FileExistsError:
        pass


def get_config_info(config=None):
    """Returns a tuple of build type and bits for the given configuration name."""
    if config[0] is None:
        raise RuntimeError('No config specified')
    if 'debug' in config[0].lower():
        bt = 'Debug'
    elif 'release' in config[0].lower():
        bt = 'Release'
    else:
        raise RuntimeError(f'Configuration name "{config[0]}" is not valid')

    bits = '32' if '32' in config else '64'
    return (bt, bits)


@task(help={'dlproject': 'Path to dlproject.yaml file (default="dlproject.yaml")',
            'config': 'Configuration name to use for building (default=Debug)',
            'update': 'Check upstream remotes to see if updates exist',
            'options': 'Conan options to pass during installation',
            'conf': 'Conan configuration options to pass during installation',
            'settings': 'Conan settings to pass during installation'},
      iterable=['config', 'options', 'conf', 'settings'],
      pre=[login])
def bootstrap(ctx, dlproject=None, config=None, update=False, options=None, conf=None, settings=None):
    """Bootstrap the project by installing dependencies and preparing the environment to build
    the C++ samples."""
    profset = env.Env()

    config_info = get_config_info(config)
    build_64_bit = '64' in config_info[1]

    dl_val_toolkit_bootstrap(ctx,
                             dlproject=dlproject,
                             config=config,
                             update=update,
                             options=options,
                             conf=conf,
                             settings=settings)
    
    igforms = 'Forms'
    igwinARM = '*_ARM64.sln'
    if build_64_bit:  # Only copy the 64-bit solution to the  64-bit staging area
        if profset.os == 'winARM':
            igwinARM = '*_64Bit.sln'
        igpat = '*_32Bit.sln'
        if profset.os == 'i80386linux' or profset.os == 'windows' or profset.os == 'armv8linux':
            igforms = ''
    else:   # Only copy the 32-bit solution to the  32-bit staging area
        igpat = '*_64Bit.sln'
    spat = shutil.ignore_patterns(
        'build', '.*', 'conan*', 'tasks', 'utils', 'python-env-*', igpat, igwinARM, igforms)
    sdir = os.path.join('build', 'CPlusPlus', 'Sample_Source')
    noerr_mkdir(sdir)
    shutil.copytree('.', sdir, ignore=spat, dirs_exist_ok=True)
    rcdir = os.path.join('build', 'Resources')
    noerr_mkdir(rcdir)
    shutil.copytree('_Input', os.path.join(
        rcdir, 'Sample_Input'), dirs_exist_ok=True)


@task(help={'config': 'Configuration name to use for building (default=Release)'})
def build(ctx, config='Release'):
    """Build the project. By default, builds 64-bit Release."""
    profset = env.Env()
    config_info = get_config_info([config])
    is_64_bit = '64' in config_info[1]
    build_type = config_info[0]

    with ctx.cd(os.path.join('build', 'CPlusPlus', 'Sample_Source', 'All')):
        shell_env = {'PATH': '/opt/freeware/bin:%s' % os.getenv('PATH')}
        if not is_64_bit:
            # set these explicitly for 32-bit platforms
            shell_env.update({'BUILD_64_BIT': "false",
                            'OS': profset.os})
        if profset.os.find('mac') > -1:
            shell_env.update({'STAGE': build_type.lower()})
            print(shell_env)
            ctx.run('gnumake', env=shell_env, echo=True)
        elif profset.os == 'windows' or profset.os == 'winARM':
            cmd = ".\\build_run_all.bat"
            if build_type == 'Release':
                cmd += " -release"
            ctx.run(cmd, env=shell_env)
        else:
            shell_env.update({'STAGE': build_type.lower()})
            print(shell_env)
            ctx.run('gmake', env=shell_env, echo=True)


tasks = []
tasks.extend([v for v in locals().values() if isinstance(v, Task)])

conan_tasks = Collection()
conan_tasks.add_task(bootstrap)
conan_tasks.add_task(login)

ns = Collection(*tasks)
ns.add_collection(conan_tasks, 'conan')

ns.configure({'run': {'echo': 'true'}})
