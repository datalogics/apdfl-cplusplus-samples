import os
from conan import ConanFile
from conan.errors import ConanException
from conan.tools.files import copy
import yaml
import shutil


class Pdfl18installerConan(ConanFile):
    name = "pdfl18_installer"
    version = "1.0.0"
    license = "Proprietary"
    url = "https://github.com/datalogics/pdfl18_installer"
    description = "Consumes adobe_pdf_library and makes installers"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        'license_managed': [True, False]
    }
    default_options = {
        'license_managed': False
    }

    def config_options(self):
        self.options['adobe_pdf_library'].license_managed = self.options.license_managed

    def _webtopdf_supported(self):
        # WebToPDF is published for Windows (x86_64 + ARM64), Linux
        # (x86_64 + ARM), and macOS (x86_64 + ARM).  Gating the
        # dependency (and the sample that uses it) keeps bootstrap from
        # failing with "no compatible configuration" on platforms where
        # no binary exists.
        os_ = str(self.settings.os)
        arch = str(self.settings.arch)
        return (os_ == "Windows" and arch in ("x86_64", "armv8")) or \
               (os_ == "Linux"   and arch in ("x86_64", "armv8")) or \
               (os_ == "Macos"   and arch in ("x86_64", "armv8"))

    @property
    def _requirements(self):
        return self.conan_data['requirements']

    def requirements(self):
        self.requires(self._requirements['adobe_pdf_library'])
        self.requires(self._requirements['apdfl-resources'])
        self.requires(self._requirements['apdfl-sample-input'])
        if self._webtopdf_supported():
            self.requires(self._requirements['webtopdf'])
        self.requires(self._requirements['installer-resources'])
        self.requires(self._requirements['tessdata'])

    def layout(self):
        self.folders.build = "build"
        self.folders.generators = self.folders.build

    def copy_apdfl(self, destination):

        apdfl_pkg = self.dependencies["adobe_pdf_library"]

        copy(self, "*.dll", src=apdfl_pkg.cpp_info.bindir, dst=destination, excludes="*DLI_PDFL*")
        if self.settings.os == "Windows":
            copy(self, "DL210PDFL.lib", src=apdfl_pkg.cpp_info.libdirs[0], dst=destination)
        copy(self, "*.ppi", src=apdfl_pkg.cpp_info.bindir, dst=destination)
        copy(self, "*", src=apdfl_pkg.cpp_info.frameworkdirs[0], dst=destination, excludes="*DLI_PDFL*")
        copy(self, "*.dylib*", src=apdfl_pkg.cpp_info.libdirs[0], dst=destination, keep_path=False)
        copy(self, "*.so*", src=apdfl_pkg.cpp_info.libdirs[0], dst=destination, keep_path=False)
        copy(self, "*.ppi*", src=apdfl_pkg.cpp_info.libdirs[0], dst=destination, keep_path=False)

        forms_ext_bin = os.path.join(apdfl_pkg.package_folder, 'formsextbin')
        forms_ext_lib = os.path.join(apdfl_pkg.package_folder, 'formsextlib')

        javascripts = os.path.join(forms_ext_bin, 'JavaScripts')
        pdfplug_ins = os.path.join(forms_ext_bin, 'pdfplug_ins')

        copy(self, '*', src=javascripts, dst=os.path.join(destination, 'JavaScripts'))
        copy(self, '*', src=pdfplug_ins, dst=os.path.join(destination, 'pdfplug_ins'))

        copy(self, "*.dll", src=forms_ext_bin, dst=destination)
        copy(self, "*.ppi", src=forms_ext_bin, dst=destination)
        if self.settings.os != "Windows":
            copy(self, "*", src=forms_ext_lib, dst=destination)

        tessdata_path = os.path.join(apdfl_pkg.cpp_info.bindir, "tessdata4")
        if os.path.isdir(tessdata_path):
            copy(self, "*", src=tessdata_path, dst=os.path.join(destination, "tessdata4"), keep_path=True)

    def copy_ocr(self, destination):
        tessdata_pkg = self.dependencies['tessdata']
        copy(self, "*", src=os.path.join(tessdata_pkg.package_folder, 'share', 'tessdata'),
             dst=os.path.join(destination, "tessdata4"),
             keep_path=True)

    def copy_webtopdf(self, destination):
        # The webtopdf conan package installs its public headers under
        # include/WebToPDF/ (namespaced by plugin name). The ConvertWebToPDF
        # sample includes them flat (e.g. "WebToPDFCalls.h") to match the
        # XPS2PDFCalls.h convention, so strip the WebToPDF/ subdir when
        # copying into the samples' include tree.
        webtopdf_pkg = self.dependencies["webtopdf"]
        webtopdf_inc = os.path.join(webtopdf_pkg.package_folder, "include", "WebToPDF")
        copy(self, "*.h", src=webtopdf_inc,
             dst="CPlusPlus/Include/Headers", keep_path=False)

        # WebToPDF's CMake target sets PREFIX "" / SUFFIX ".ppi", so the
        # plugin is named WebToPDF.ppi on every platform.  CMake routes
        # the file via RUNTIME on Windows (lands in bin/) and LIBRARY on
        # Unix (lands in lib/), so copy from both candidate directories
        # to cover the platform layout difference.
        copy(self, "WebToPDF.ppi", src=webtopdf_pkg.cpp_info.bindir,
             dst=destination, keep_path=False)
        copy(self, "WebToPDF.ppi", src=webtopdf_pkg.cpp_info.libdirs[0],
             dst=destination, keep_path=False)

    def _imports(self):
        pdfl_pkg_inc = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'include')
        pdfl_pkg_src = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'src')
        copy(self, '*', src=pdfl_pkg_inc, dst='CPlusPlus/Include/Headers',
             excludes=['CAXE*.h', 'axe*.h', 'OBIB.h'])
        copy(self, 'PDFLInit*', src=pdfl_pkg_src, dst='CPlusPlus/Include/Source')

        # APDFL 21+ ships the runtime Resources directory as a separate
        # apdfl-resources package; content lives under Resources/ inside
        # the package folder.
        apdfl_rsc = os.path.join(
            self.dependencies['apdfl-resources'].package_folder, 'Resources')
        if not os.path.isdir(apdfl_rsc):
            raise ConanException(
                f'apdfl-resources package does not contain a Resources/ '
                f'directory at {apdfl_rsc}'
            )
        copy(self, '*', src=apdfl_rsc, dst='Resources')

        # APDFL 21+ also ships the sample-input data as its own
        # apdfl-sample-input package; content lives under Sample_Input/.
        sample_input_src = os.path.join(
            self.dependencies['apdfl-sample-input'].package_folder, 'Sample_Input')
        if not os.path.isdir(sample_input_src):
            raise ConanException(
                f'apdfl-sample-input package does not contain a Sample_Input/ '
                f'directory at {sample_input_src}'
            )
        copy(self, '*', src=sample_input_src, dst='Resources/Sample_Input')

        self.copy_apdfl(destination='CPlusPlus/Binaries')
        self.copy_ocr(destination='CPlusPlus/Binaries')
        if self._webtopdf_supported():
            self.copy_webtopdf(destination='CPlusPlus/Binaries')


    def generate(self):
        self._imports()

    def source(self):
        pass

    def build(self):
        pass

    def package(self):
        pass

    def package_info(self):
        pass