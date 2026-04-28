import os
from conan import ConanFile
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
        # WebToPDF is published only for Windows x86_64, Linux x86_64, and
        # Linux ARM. Gating the dependency (and the sample that uses it)
        # keeps bootstrap from failing with "no compatible configuration"
        # on platforms where no binary exists.
        os_ = str(self.settings.os)
        arch = str(self.settings.arch)
        return (os_ == "Windows" and arch == "x86_64") or \
               (os_ == "Linux"   and arch in ("x86_64", "armv8"))

    def requirements(self):
        self.requires("adobe_pdf_library/[>=18.0.5+ <21.0.0]@datalogics/nightly")
        if self._webtopdf_supported():
            self.requires("webtopdf/[>=1.0.0]@datalogics/nightly")
        self.requires("installer-resources/[>=0.7]@datalogics/stable")
        self.requires(self.conan_data['tessdata'])

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

        # Runtime: the plugin .ppi (Windows) lives in bin/, the shared
        # library (libWebToPDF.dylib / libWebToPDF.so / WebToPDF.dll) lives
        # in lib/ on Unix and bin/ on Windows. Copy both so ASExtensionMgrGetHFT
        # can locate the plugin at runtime.
        copy(self, "*.ppi", src=webtopdf_pkg.cpp_info.bindir, dst=destination)
        copy(self, "WebToPDF.dll", src=webtopdf_pkg.cpp_info.bindir, dst=destination)
        copy(self, "libWebToPDF.*", src=webtopdf_pkg.cpp_info.libdirs[0],
             dst=destination, keep_path=False)

    def _imports(self):
        pdfl_pkg_inc = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'include')
        pdfl_pkg_src = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'src')
        pdfl_pkg_rsc = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'Resources')
        copy(self, '*', src=pdfl_pkg_inc, dst='CPlusPlus/Include/Headers',
             excludes=['CAXE*.h', 'axe*.h', 'OBIB.h'])
        copy(self, 'PDFLInit*', src=pdfl_pkg_src, dst='CPlusPlus/Include/Source')
        copy(self, "*", src=pdfl_pkg_rsc, dst='Resources')
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