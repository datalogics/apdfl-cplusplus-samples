@Library('jenkins-shared-libraries') _
def ENV_LOC=[:]
def EXTRA_ARGS = [
    'linux-apdfl-rocky-x64-samples': ' ',
    'linux-armv8-apdfl-samples': ' ',
    'mac-apdfl-samples': ' ',
    'mac-arm-apdfl-samples': ' ',
    'sparcsolaris-apdfl-samples': ' ',
    'windows-apdfl-samples': ' ',
    'windows-ARM-apdfl-samples': ' ',
]
pipeline {
    parameters {
        choice(name: 'PLATFORM_FILTER', choices: ['all', 'mac-apdfl-samples', 'mac-arm-apdfl-samples', 'linux-armv8-apdfl-samples', 'linux-apdfl-rocky-x64-samples', 'windows-apdfl-samples', 'windows-ARM-apdfl-samples'], description: 'Run on specific platform')
    }
    options{
        timeout(time: 1, unit: "HOURS")
    }
    agent none
    triggers {
        parameterizedCron(env.BRANCH_NAME == "develop-18" ? "0 8 * * *" : "")
    }
    stages {
        stage('Matrix stage') {
            matrix {
                agent {
                    label "${NODE}"
                }
                when { anyOf {
                    expression { params.PLATFORM_FILTER == 'all' }
                    expression { params.PLATFORM_FILTER == env.NODE }
                } }
                axes {
                    axis {
                        name 'NODE'
                        values 'mac-apdfl-samples', 'mac-arm-apdfl-samples', 'linux-armv8-apdfl-samples', 'windows-apdfl-samples', 'windows-ARM-apdfl-samples', 'linux-apdfl-rocky-x64-samples'
                    }
                    axis {
                        name 'BITS'
                        values '64', '32'
                    }
                }
                excludes {
                    exclude {
                        axis {
                            name 'NODE'
                            values 'mac-apdfl-samples', 'mac-arm-apdfl-samples', 'linux-armv8-apdfl-samples', 'windows-ARM-apdfl-samples', 'linux-apdfl-rocky-x64-samples'
                        }
                        axis {
                            name 'BITS'
                            values '32'
                        }
                    }
                }
                environment {
                    CONAN_USER_HOME = "${WORKSPACE}"
                    CONAN_NON_INTERACTIVE = '1'
                    CONAN_PRINT_RUN_COMMANDS = '1'
                    CONAN_LOGIN_USERNAME='devauto'
                    CONAN_PASSWORD=credentials('jfrog-dev-token')
                    // Disable FileTracker on Windows, which can give FTK1011 on long path names
                    TRACKFILEACCESS = 'false'
                    // Disable node reuse, which gives intermittent build errors on Windows
                    MSBUILDDISABLENODEREUSE = '1'
                    SKIPPLATFORM = setSkipPlatform(NODE)
                }
                stages {
                    stage('Remove Conan local cache') {
                        when {
                            anyOf {
                                expression { "${SKIPPLATFORM}" == 'false' }
                                not {
                                    changeRequest()
                                }

                            }
                        }
                        steps {
                            echo "Remove Conan local cache ${NODE} ${BITS}"
                            script {
                                if (isUnix()) {
                                    sh "rm -rf ${WORKSPACE}/.conan"
                                } else {
                                    bat "if exist ${WORKSPACE}\\.conan\\ rmdir/s/q ${WORKSPACE}\\.conan"
                                }
                            }
                        }
                    }
                    stage('Set-Up Environment') {
                        when {
                            expression { "${SKIPPLATFORM}" == 'false' }
                            }
                        steps {
                            printPlatformNameInStep()
                            echo "Set-Up Environment ${NODE} ${BITS}"
                            script {
                                ENV_LOC["${NODE}_${BITS}"] = mkenv()
                            }
                        }
                    }
                    stage('Clean') {
                        when {
                            expression { "${SKIPPLATFORM}" == 'false' }
                            }
                        steps {
                            echo "Bootstrap ${NODE} ${BITS}"
                            script {
                                if (isUnix()) {
                                    sh """. ${ENV_LOC["${NODE}_${BITS}"]}/bin/activate
                                       unset LIBPATH
                                          invoke distclean
                                    """
                                } else {
                                    bat """CALL ${ENV_LOC["${NODE}_${BITS}"]}\\Scripts\\activate
                                          invoke distclean
                                    """
                                }
                            }
                        }
                    }
                    stage('Bootstrap') {
                        when {
                            expression { "${SKIPPLATFORM}" == 'false' }
                        }
                        steps {
                            echo "Bootstrap ${NODE} ${BITS}"
                            script {
                                    bootstrapConfigs([licenseManaged: "False",
                                                    pythonEnv: "${ENV_LOC[NODE]}",
                                                    buildType: "Release",
                                                    extraArguments: "${EXTRA_ARGS[NODE]}"])
                            }
                        }
                    }
                    stage('Build') {
                        when {
                            expression { "${SKIPPLATFORM}" == 'false' }
                        }
                        steps {
                            echo "Build ${NODE} ${BITS}"
                            script {
                                if (isUnix()) {
                                    sh """. ${ENV_LOC["${NODE}_${BITS}"]}/bin/activate
                                       unset LIBPATH
                                          invoke build --config Release${EXTRA_ARGS[NODE]}
                                    """
                                } else {
                                    bat """CALL ${ENV_LOC["${NODE}_${BITS}"]}\\Scripts\\activate
                                          invoke build --config Release${EXTRA_ARGS[NODE]}
                                    """
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    post {
        unsuccessful {
            script {
                    if (env.CHANGE_ID == null) {  // i.e. not a pull request; those notify in GitHub
                        slackSend(channel: "#apdfl-18",
                                message: "Unsuccessful build: ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)",
                                color: "danger")
                    }
                }
            }
            fixed {
                script {
                    if (env.CHANGE_ID == null) {  // i.e. not a pull request; those notify in GitHub
                        slackSend(channel: "#apdfl-18",
                                message: "Build is now working: ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)",
                                color: "good")
                }
            }
        }
    }
}

//AIX will only run if specific from the platform filter in the Jenkins UI.
def setSkipPlatform(String node) {
    if ("${node}" == 'aix-apdfl-samples') {
        if (params.PLATFORM_FILTER == 'aix-apdfl-samples') {
            return 'false'
        }
        return 'true'
    }
    return 'false'
}

void printPlatformNameInStep() {
    script {
        stage("${node} - ${bits}") {
            echo "Building: ${node}"
        }
    }
}
