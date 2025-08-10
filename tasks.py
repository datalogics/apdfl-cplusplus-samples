import os
import shutil
import platform
import re
import fnmatch
from dl_conan_build_tools.tasks import conan
from dl_conan_build_tools.config import get_config
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


@task(help={'options': 'Options to pass to "conan install" (can be specified more than once)',
            'build-type': 'Type of the build, usually Debug or Release',
            'bits': '64 or 32 (defaults to 64)',
            'update': 'Check upstream remotes to see if updates exist',
            'offline': 'Work offline, won\'t install config or login',
        },
      iterable=['options'])
def bootstrap(ctx, options=None, build_type='Release', bits='64', update=False, offline=False):
    """Ready the project for building.
    Unless '--offline', log into Conan, get the config, and set up remotes.

    Then, create the build directory tree, copy files, and install dependencies.
    If the dependencies can't be satisfied by the Conan client cache, a network
    connection is required to search Artifactory.
    """
    profset = env.Env()
    if not offline:
        conan.install_config(ctx)
        conan.login(ctx)
    install_folder = profset.install_folder(build_type, build_64_bit=(bits == '64'))
    
    igforms = 'Forms'
    igwinARM = '*_ARM64.sln'
    if bits == '64':  # Only copy the 64-bit solution to the  64-bit staging area
        if profset.os == 'winARM':
            igwinARM = '*_64Bit.sln'
        igpat = '*_32Bit.sln'
        if profset.os == 'i80386linux' or profset.os == 'windows' or profset.os == 'armv8linux':
            igforms = ''
    else:   # Only copy the 32-bit solution to the  32-bit staging area
        igpat = '*_64Bit.sln'
    spat = shutil.ignore_patterns(
        install_folder, '.*', 'conan*', 'tasks', 'utils', 'python-env-*', igpat, igwinARM, igforms)
    sdir = os.path.join(install_folder, 'CPlusPlus', 'Sample_Source')
    noerr_mkdir(sdir)
    shutil.copytree('.', sdir, ignore=spat, dirs_exist_ok=True)
    rcdir = os.path.join(install_folder, 'Resources')
    noerr_mkdir(rcdir)
    shutil.copytree('_Input', os.path.join(
        rcdir, 'Sample_Input'), dirs_exist_ok=True)
    settings = ['-s build_type=' + build_type]
    cdict = get_config()
    profile = os.path.join('profiles', install_folder)
    settings += [f'--profile={profile}']
    ctx.run(
        f'conan install {" ".join(settings)} -if {install_folder} .', echo=True)


@task(help={'build-type': 'Type of the build, usually Debug or Release',
            'bits': '64 or 32 (defaults to 64)',
        })
def build(ctx, build_type='Release', bits='64'):
    """Build the project. By default, builds 64-bit Release.

    For more information on flags that can be repeated, see
    http://docs.pyinvoke.org/en/stable/concepts/invoking-tasks.html#iterable-flag-values"""
    profset = env.Env()
    is_64_bit = bits == '64'
    install_folder = profset.install_folder(build_type, is_64_bit)
    with ctx.cd(os.path.join(install_folder, 'CPlusPlus', 'Sample_Source', 'All')):
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
conan_tasks.add_task(conan.install_config)
conan_tasks.add_task(conan.login)

ns = Collection(*tasks)
ns.add_collection(conan_tasks, 'conan')

ns.configure({'run': {'echo': 'true'}})
