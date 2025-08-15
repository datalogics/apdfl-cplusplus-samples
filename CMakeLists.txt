
################################################################################
# Install resources into "dist" for APDFL 18
################################################################################

set(DL_SAMPLES Sample_Source)

if(DEFINED ENV{DL_MAKING_CONAN_PACKAGE})
  set(RES_INSTALL_DIR ${INSTALL_RESOURCEDIR})
else()
  set(RES_INSTALL_DIR dist/${CMAKE_BUILD_TYPE_CAMEL}/Resources)
endif()

if (NOT (UTIL_PLAT MATCHES "x64") AND NOT (UTIL_PLAT MATCHES "ARM64") AND NOT (UTIL_PLAT MATCHES "Win32") AND NOT (UTIL_PLAT MATCHES "i80386linux_64") AND NOT (UTIL_PLAT MATCHES "armv8linux_64"))
install(DIRECTORY    ${CMAKE_CURRENT_SOURCE_DIR}/_Input/
        DESTINATION  "${RES_INSTALL_DIR}/Sample_Input"
        USE_SOURCE_PERMISSIONS
        PATTERN "CMakeLists.txt" EXCLUDE
        PATTERN "Word.pdf" EXCLUDE
        PATTERN "Excel.pdf" EXCLUDE
        PATTERN "PowerPoint.pdf" EXCLUDE
        )
else()
install(DIRECTORY    ${CMAKE_CURRENT_SOURCE_DIR}/_Input/
        DESTINATION  "${RES_INSTALL_DIR}/Sample_Input"
        USE_SOURCE_PERMISSIONS
        PATTERN "CMakeLists.txt" EXCLUDE
        )
endif()

if(UTIL_PLAT MATCHES "x64")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*32Bit*.*" EXCLUDE PATTERN "*.sh" EXCLUDE PATTERN "*Makefile*" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.rel*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "dlutils" EXCLUDE
            PATTERN "*.mak" EXCLUDE PATTERN "*ARM64.*" EXCLUDE PATTERN "conandata.yml" EXCLUDE
            PATTERN "conanfile.py" EXCLUDE PATTERN "dlproject.yaml" EXCLUDE
            PATTERN "Jenkinsfile" EXCLUDE PATTERN "mkenv.py" EXCLUDE PATTERN "utils" EXCLUDE
            PATTERN "requirements.in" EXCLUDE PATTERN "profiles" EXCLUDE PATTERN "tasks" EXCLUDE
            PATTERN "Public_Repo_Update.py" EXCLUDE
            )
elseif(UTIL_PLAT MATCHES "ARM64")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*Bit*.*" EXCLUDE PATTERN "*.sh" EXCLUDE PATTERN "*Makefile*" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.rel*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "dlutils" EXCLUDE
            PATTERN "*.mak" EXCLUDE PATTERN "conandata.yml" EXCLUDE
            PATTERN "conanfile.py" EXCLUDE PATTERN "dlproject.yaml" EXCLUDE
            PATTERN "Jenkinsfile" EXCLUDE PATTERN "mkenv.py" EXCLUDE PATTERN "utils" EXCLUDE
            PATTERN "requirements.in" EXCLUDE PATTERN "profiles" EXCLUDE PATTERN "tasks" EXCLUDE
            PATTERN "Public_Repo_Update.py" EXCLUDE
            )
elseif(UTIL_PLAT MATCHES "Win32")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*64Bit*.*" EXCLUDE PATTERN "*.sh" EXCLUDE PATTERN "*Makefile*" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.rel*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "dlutils" EXCLUDE
            PATTERN "*.mak" EXCLUDE PATTERN "*ARM64.*" EXCLUDE PATTERN "conandata.yml" EXCLUDE
            PATTERN "conanfile.py" EXCLUDE PATTERN "dlproject.yaml" EXCLUDE
            PATTERN "Jenkinsfile" EXCLUDE PATTERN "mkenv.py" EXCLUDE PATTERN "utils" EXCLUDE
            PATTERN "requirements.in" EXCLUDE PATTERN "profiles" EXCLUDE PATTERN "tasks" EXCLUDE
            PATTERN "Public_Repo_Update.py" EXCLUDE
            )
elseif(UTIL_PLAT MATCHES "Mac" OR UTIL_PLAT MATCHES "armv8mac_64")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*Bit*.*" EXCLUDE PATTERN "*.bat" EXCLUDE PATTERN "*.vcxproj*" EXCLUDE
            PATTERN "*.sh" EXCLUDE PATTERN "*ARM64.*" EXCLUDE
            PATTERN "PDFViewer" EXCLUDE
            PATTERN "ConvertToOffice" EXCLUDE PATTERN "conandata.yml" EXCLUDE
            PATTERN "conanfile.py" EXCLUDE PATTERN "dlproject.yaml" EXCLUDE
            PATTERN "Jenkinsfile" EXCLUDE PATTERN "mkenv.py" EXCLUDE PATTERN "utils" EXCLUDE
            PATTERN "requirements.in" EXCLUDE PATTERN "profiles" EXCLUDE PATTERN "tasks" EXCLUDE
            PATTERN "Public_Repo_Update.py" EXCLUDE
            )
elseif(UTIL_PLAT MATCHES "i80386linux_64" OR UTIL_PLAT MATCHES "armv8linux_64")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.bat*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "*.sh*" EXCLUDE
            PATTERN "*Bit*.*" EXCLUDE PATTERN "*.vcxproj*" EXCLUDE PATTERN "*ARM64.*" EXCLUDE
            PATTERN "PDFPrintGUI" EXCLUDE PATTERN "PDFViewer" EXCLUDE
            PATTERN "conanfile.py" EXCLUDE PATTERN "dlproject.yaml" EXCLUDE
            PATTERN "Jenkinsfile" EXCLUDE PATTERN "mkenv.py" EXCLUDE PATTERN "utils" EXCLUDE
            PATTERN "requirements.in" EXCLUDE PATTERN "profiles" EXCLUDE PATTERN "tasks" EXCLUDE
            PATTERN "Public_Repo_Update.py" EXCLUDE PATTERN "conandata.yml" EXCLUDE
            )
else()
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
            DESTINATION ${INSTALL_SAMPLEDIR}/${DL_SAMPLES}
            USE_SOURCE_PERMISSIONS
            PATTERN ".git*" EXCLUDE PATTERN "_Input" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE
            PATTERN "*.xcodeproj" EXCLUDE PATTERN "*.bat*" EXCLUDE PATTERN "*Mac*" EXCLUDE
            PATTERN "*.plist" EXCLUDE PATTERN "*.strings*" EXCLUDE PATTERN "*.sh*" EXCLUDE
            PATTERN "*Bit*.*" EXCLUDE PATTERN "*.vcxproj*" EXCLUDE PATTERN "*ARM64.*" EXCLUDE
            PATTERN "PDFPrintGUI" EXCLUDE PATTERN "PDFViewer" EXCLUDE
            PATTERN "ConvertToOffice" EXCLUDE PATTERN "conandata.yml" EXCLUDE
            PATTERN "conanfile.py" EXCLUDE PATTERN "dlproject.yaml" EXCLUDE
            PATTERN "Jenkinsfile" EXCLUDE PATTERN "mkenv.py" EXCLUDE PATTERN "utils" EXCLUDE
            PATTERN "requirements.in" EXCLUDE PATTERN "profiles" EXCLUDE PATTERN "tasks" EXCLUDE
            PATTERN "Public_Repo_Update.py" EXCLUDE
            )
endif()
