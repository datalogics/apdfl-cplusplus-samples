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

    def requirements(self):
        self.requires("adobe_pdf_library/[>=18.0.5+]@datalogics/nightly")
        self.requires("installer-resources/[>=0.7]@datalogics/stable")

    def layout(self):
        self.folders.build = "build"
        self.folders.generators = self.folders.build

    def copy_apdfl(self, destination):

        apdfl_pkg = self.dependencies["adobe_pdf_library"]

        copy(self, "*.dll", src=apdfl_pkg.cpp_info.bindir, dst=destination, excludes="*DLI_PDFL*")
        if self.settings.os == "Windows":
            copy(self, "DL180PDFL.lib", src=apdfl_pkg.cpp_info.libdirs[0], dst=destination)
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

    def _imports(self):
        pdfl_pkg_inc = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'include')
        pdfl_pkg_src = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'src')
        pdfl_pkg_rsc = os.path.join(self.dependencies["adobe_pdf_library"].package_folder, 'Resources')
        copy(self, '*', src=pdfl_pkg_inc, dst='CPlusPlus/Include/Headers',
             excludes=['CAXE*.h', 'axe*.h', 'OBIB.h'])
        copy(self, 'PDFLInit*', src=pdfl_pkg_src, dst='CPlusPlus/Include/Source')
        copy(self, "*", src=pdfl_pkg_rsc, dst='Resources')
        self.copy_apdfl(destination='CPlusPlus/Binaries')


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