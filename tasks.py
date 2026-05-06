import glob
import os
import shutil
import subprocess
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

    # WebToPDF plugin ships for 64-bit Windows (x64 + ARM64), 64-bit Linux
    # (x86_64 + ARM), and both macOS architectures.  Anywhere else the
    # sample is kept out of the staging tree so the build phase never
    # tries to compile it.
    webtopdf_supported = build_64_bit and profset.os in (
        'windows', 'winARM', 'i80386linux', 'armv8linux', 'mac-x86-64', 'armv8mac',
    )
    ignore_webtopdf = () if webtopdf_supported else ('ConvertWebToPDF',)

    spat = shutil.ignore_patterns(
        'build', '.*', 'conan*', 'tasks', 'utils', 'python-env-*',
        igpat, igwinARM, igforms, *ignore_webtopdf)
    sdir = os.path.join('build', 'CPlusPlus', 'Sample_Source')
    noerr_mkdir(sdir)
    shutil.copytree('.', sdir, ignore=spat, dirs_exist_ok=True)
    rcdir = os.path.join('build', 'Resources')
    noerr_mkdir(rcdir)
    shutil.copytree('_Input', os.path.join(
        rcdir, 'Sample_Input'), dirs_exist_ok=True)


_WIN_SLN_BY_ARCH = {
    'ARM64': 'All_Datalogics_ARM64.sln',
    'Win32': 'All_Datalogics_32Bit.sln',
    'x64': 'All_Datalogics_64Bit.sln',
}
_WIN_FE_SLN = 'All_DatalogicsFE_64Bit.sln'

# Samples that build fine but cannot run unattended on CI: GUI apps, ones
# that block on stdin, and ones that bail out with usage when no
# command-line argument is supplied.
_WIN_SKIP_RUN = {
    'Display/PDFViewer',                    # MFC GUI viewer
    'DocumentConversion/ConvertToFactur-X',  # requires input PDF arg
    'DocumentConversion/ConvertToZUGFeRD',   # requires input PDF + XML args
    'Printing/PDFPrintDefault',             # drives the OS default printer
    'Printing/PDFPrintGUI',                 # opens the OS print dialog
    'Text/InsertHeadFoot',                  # reads a password from stdin
}

def _find_msbuild():
    vswhere = os.path.join(
        os.environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)'),
        'Microsoft Visual Studio', 'Installer', 'vswhere.exe',
    )
    if not os.path.isfile(vswhere):
        raise RuntimeError(f'vswhere.exe not found at {vswhere!r}')
    out = subprocess.check_output(
        [vswhere, '-latest', '-products', '*',
         '-requires', 'Microsoft.Component.MSBuild',
         '-find', r'MSBuild\**\Bin\MSBuild.exe'],
        text=True,
    )
    paths = [p for p in out.splitlines() if p.strip()]
    if not paths:
        raise RuntimeError(f'vswhere returned no MSBuild path:\n{out}')
    return paths[0]


def _windows_build_and_run(ctx, build_type, is_64_bit):
    """Build every staged Windows sample solution and run every executable
    that gets produced.  Raises on any build/run failure so CI surfaces
    real problems instead of staying green on a no-op."""
    sample_root = os.path.join('build', 'CPlusPlus', 'Sample_Source')
    all_dir = os.path.join(sample_root, 'All')

    real = (os.environ.get('PROCESSOR_ARCHITEW6432')
            or os.environ.get('PROCESSOR_ARCHITECTURE', ''))
    if 'ARM' in real.upper():
        arch = 'ARM64'
    elif not is_64_bit:
        arch = 'Win32'
    else:
        arch = 'x64'
    msbuild = _find_msbuild()
    print(f'arch={arch} configuration={build_type} msbuild={msbuild}',
          flush=True)

    main_sln = _WIN_SLN_BY_ARCH[arch]
    main_sln_path = os.path.join(all_dir, main_sln)
    if not os.path.isfile(main_sln_path):
        raise RuntimeError(
            f'{main_sln} is missing from {all_dir}; bootstrap did not '
            f'stage the {arch} solution.'
        )

    fe_sln_path = os.path.join(all_dir, _WIN_FE_SLN)
    solutions = [main_sln_path]
    if os.path.isfile(fe_sln_path):
        solutions.append(fe_sln_path)

    failed_builds = []
    for sln_path in solutions:
        result = ctx.run(
            f'"{msbuild}" "{sln_path}" /p:Configuration={build_type} '
            f'/p:Platform={arch} /t:Rebuild /m /nologo /v:minimal',
            warn=True,
        )
        if result.exited != 0:
            failed_builds.append(os.path.basename(sln_path))

    pattern = os.path.join(sample_root, '*', '*', arch, build_type, '*.exe')
    exes = sorted(glob.glob(pattern))
    print(f'\nDiscovered {len(exes)} executable(s) to run', flush=True)
    if not exes:
        raise RuntimeError(
            f'No sample executables found under {sample_root} matching '
            f'{arch}/{build_type}; the build produced nothing to run.'
        )

    failed_runs = []
    skipped = []
    for exe in exes:
        sample_dir = os.path.dirname(os.path.dirname(os.path.dirname(exe)))
        rel = os.path.relpath(sample_dir, sample_root).replace(os.sep, '/')
        if rel in _WIN_SKIP_RUN:
            print(f'\n==== SKIP {rel} (not runnable on CI) ====', flush=True)
            skipped.append(rel)
            continue
        print(f'\n==== RUN {rel} ====', flush=True)
        rc = subprocess.call([exe], cwd=sample_dir)
        if rc != 0:
            failed_runs.append(f'{rel} (exit {rc})')

    print('\n==== SUMMARY ====')
    print(f'arch={arch} configuration={build_type}')
    print(f'samples ran:     {len(exes) - len(skipped)}')
    print(f'samples skipped: {len(skipped)}')
    print(f'samples failed:  {len(failed_runs)}')
    for sln in failed_builds:
        print(f'  build failed: {sln}')
    for r in failed_runs:
        print(f'  run failed:   {r}')
    if failed_builds or failed_runs:
        raise RuntimeError(
            f'{len(failed_builds)} solution build failure(s), '
            f'{len(failed_runs)} run failure(s)'
        )


@task(help={'config': 'Configuration name to use for building (default=Release)'})
def build(ctx, config='Release'):
    """Build the project. By default, builds 64-bit Release."""
    profset = env.Env()
    config_info = get_config_info([config])
    is_64_bit = '64' in config_info[1]
    build_type = config_info[0]

    if profset.os in ('windows', 'winARM'):
        _windows_build_and_run(ctx, build_type, is_64_bit)
        return

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
