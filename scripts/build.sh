#!/bin/bash
set -eE
trap 'error ${LINENO}' ERR
#
# Default build options
#
opt_build=all
opt_verbose=
opt_buildtype=
opt_format=
opt_cmake_ver="3.13"

#
# Get script name and path
#
script_name=$0
script_full=$(realpath "${script_name}")
script_dir=$(dirname "${script_full}")

function pr_err()
{
    printf "\033[1;31m\n"
    printf "%s" "$@"
    printf "\033[0m\n"
}

function error() {
    pr_err "Error occurred at line $1"
    exit 1
}

function pr_operation_header() {
    local message="$*"

    printf "\033[0;32m\n"
    printf "***************************************\n"
    if [[ -n "$message" ]]; then
        printf " %s\n" "${message}"
    fi
    printf "***************************************\n"
    printf "\033[0m\n"
}

function check_cmake_version()
{
    #
    # Check if cmake is installed and minimum version is expected
    #
    if [[ ! -x $(command -v cmake) ]]; then
        pr_err "cmake not installed\n"
        pr_err "cmake version required is minimum ${opt_cmake_ver}\n"
        exit 1
    fi

    # Get the current cmake version installed
    cmake_ver=$(cmake -version 2>&1 | grep -Eo "cmake version [0-9]\.[0-9]+")
    cmake_ver=$(echo "${cmake_ver}" | grep -Eo '[0-9]\.[0-9]+')

    IFS='.' read -ra acmake_ver <<< "${cmake_ver}"
    IFS='.' read -ra aexp_ver <<< "${opt_cmake_ver}"

    match=${#aexp_ver[@]}
    if [[ ${#acmake_ver[@]} -eq ${#aexp_ver[@]} ]]; then
        for (( i=0; i<${#aexp_ver[@]}; i++ )); do
            if [[ ${acmake_ver[$i]} -lt ${aexp_ver[$i]} ]]; then
                break;
            fi
            match=$((match-1))
        done
    fi

    if [[ ${match} -ne 0 ]]; then
        printf "Bad cmake version got %s expected %s or newer\n" \
               "${cmake_ver}" "${opt_cmake_ver}"
        exit 1
    fi
}

function check_directory()
{
    declare -n mydir=$1
    mydir="${mydir/\~/$HOME}"

    if [[ ! -d "${mydir}" ]]; then
        pr_err "${mydir} is not a directory"
    fi
}

function usage_toolchain()
{
    printf "\n"
    printf "To install the toolchain\n"
    printf "  %s toolchain toolpath=[dir] " "${script_name}"
    printf "toolname=[name]\n"
    printf "    toolpath = [optional] Toolchain installation path\n"
    printf "    toolname = [optional] Toolchain name\n"
    printf "\n"
}

function usage_yaml()
{
    printf "\n"
    printf "To build and install the Yaml Library\n"
    printf "  %s yaml export=[dir] src=[dir] " "${script_name}"
    printf "toolpath=[dir] toolname=[name]\n"
    printf "    export   = Export directory\n"
    printf "    src      = Temporary directory where to install sources\n"
    printf "    toolpath = [optional] Toolchain installation path\n"
    printf "    toolname = [optional] Toolchain name\n"
    printf "    version  = [optional] Libyaml version to download\n"
    printf "    hash     = [optional] Libyaml Hash of archive to unload\n"
    printf "\n"
}

function usage_cyaml()
{
    printf "\n"
    printf "To build and install the Cyaml Library\n"
    printf "  %s cyaml export=[dir] src=[dir] " "${script_name}"
    printf "toolpath=[dir] toolname=[name]\n"
    printf "    export   = Export directory\n"
    printf "    src      = Temporary directory where to install sources\n"
    printf "    toolpath = [optional] Toolchain installation path\n"
    printf "    toolname = [optional] Toolchain name\n"
    printf "    branch   = [optional] Libcyaml branch to get\n"
    printf "    version  = [optional] Libcyaml revision to get\n"
    printf "\n"
}

function usage_mbedtls()
{
    printf "\n"
    printf "To build and install the MbedTLS Library\n"
    printf "  %s mbedtls export=[dir] src=[dir] out=[dir] " "${script_name}"
    printf "toolpath=[dir] toolname=[name]\n"
    printf "    export   = Export directory\n"
    printf "    src      = Temporary directory where to install sources\n"
    printf "    out      = Temporary directory where to build sources\n"
    printf "    toolpath = [optional] Toolchain installation path\n"
    printf "    toolname = [optional] Toolchain name\n"
    printf "    branch   = [optional] MbedTLS branch to get\n"
    printf "    revision = [optional] MBeTLS revision to get\n"
    printf "\n"
}

function usage_smw()
{
    printf "\n"
    printf "To build and install the SMW Library\n"
    printf "  %s smw export=[dir] src=[dir] out=[dir]" "${script_name}"
    printf " toolpath=[dir] toolname=[name]\n"
    printf "    export   = Export directory\n"
    printf "    src      = Temporary directory where to install sources\n"
    printf "    out      = Install directory\n"
    printf "    toolpath = [optional] Toolchain installation path\n"
    printf "\n"
}

function usage_el2go_agent()
{
    printf "\n"
    printf "To build and install the EdgeLock 2GO agent Library\n"
    printf "  %s el2go_agent export=[dir] src=[dir] out=[dir] smw=[dir] mbetls=[dir] " "${script_name}"
    printf "toolpath=[dir] toolname=[name]\n"
    printf "    export   = Export directory\n"
    printf "    src      = Temporary directory where to install sources\n"
    printf "    out      = Temporary directory where to build sources\n"
    printf "    smw      = Folder where SMW cmake config files are located\n"
    printf "    mbedtls  = Folder where MbedTLS cmake config files are located\n"
    printf "    toolpath = [optional] Toolchain installation path\n"
    printf "    toolname = [optional] Toolchain name\n"
    printf "\n"
}

function usage_configure()
{
    printf "\n"
    printf "To configure the OEM Provisioning Application\n"
    printf " - Note: all dependencies must be present\n"
    printf "  %s configure out=[dir] " "${script_name}"
    printf "cyaml=[dir] mbedtls=[dir] el2go_agent=[dir]\n"
    printf "    out          = Build directory\n"
    printf "    cyaml        = Cyaml package config folder\n"
    printf "    mbedtls      = MbedTLS cmake config files\n"
    printf "    el2go_agent  = EL2GO Agent cmake config files\n"
    printf "\n"
}

function usage_build()
{
    printf "\n"
    printf "To build the OEM Provisioning Application\n"
    printf " - Note: Project must have been configure first\n"
    printf " (ref. %s configure)\n" "${script_name}"
    printf "  %s build out=[dir]" "${script_name}"
    printf "\n"
    printf "    out    = Build directory\n"
}

function usage_install()
{
    printf "\n"
    printf "To install the Security OEM Provisioning Application binaries\n"
    printf "  %s install out=[dir] dest=[dir]\n" "${script_name}"
    printf "    out      = Build directory\n"
    printf "    dest     = [optional] Installation directory\n"
    printf "\n"
}

function usage()
{
    printf "\n"
    printf "****************************************************\n"
    printf " Usage of OEM Provisioning Application build script \n"
    printf "****************************************************\n"
    usage_toolchain
    usage_yaml
    usage_cyaml
    usage_mbedtls
    usage_smw
    usage_el2go_agent
    usage_configure
    usage_build
    usage_install
    exit 1
}

function toolchain()
{
    cmd_script="cmake -DFORCE_TOOLCHAIN_INSTALL=True"
    cmd_script="${cmd_script} ${opt_toolpath} ${opt_toolname}"

    pr_operation_header "Install toolchain"

    cmd_script="${cmd_script} -P ${toolchain_script}"
    printf "Execute %s\n" "${cmd_script}"
    eval "${cmd_script}"
}

function yaml()
{
    cmd_script="cmake ${opt_toolchain}"
    yaml_script="${script_dir}/build_yaml.cmake"

    pr_operation_header "Install yaml to ${opt_export}"

    if [[ -z ${opt_export} || -z ${opt_src} ]]; then
        usage_yaml
        exit 1
    fi

    cmd_script="${cmd_script} -DYAML_SRC_PATH=${opt_src}"
    cmd_script="${cmd_script} -DYaml_ROOT=${opt_export} -P ${yaml_script}"

    if [[ ${opt_version} && ${opt_hash} ]]; then
        cmd_script="${cmd_script} -DYAML_VERSION=${opt_version}"
        cmd_script="${cmd_script} -DYAML_HASH=${opt_hash}"
    fi

    printf "Execute %s\n" "${cmd_script}"
    eval "${cmd_script}"
}

function cyaml()
{
    cmd_script="cmake ${opt_toolchain}"
    cyaml_script="${script_dir}/build_cyaml.cmake"

    pr_operation_header "Install Cyaml to ${opt_export}"

    if [[ -z ${opt_export} || -z ${opt_src} ]]; then
        usage_cyaml
        exit 1
    fi

    cmd_script="${cmd_script} -DCYAML_SRC_PATH=${opt_src}"
    # we need this in order to find the yaml libraries
    cmd_script="${cmd_script} -DCMAKE_PREFIX_PATH=${opt_export}"
    cmd_script="${cmd_script} -DCyaml_ROOT=${opt_export} -P ${cyaml_script}"

    if [[ ${opt_branch} && ${opt_version} ]]; then
        cmd_script="${cmd_script} -DCYAML_BRANCH=${opt_branch}"
        cmd_script="${cmd_script} -DCYAML_REVISION=${opt_version}"
    fi

    printf "Execute %s\n" "${cmd_script}"
    eval "${cmd_script}"
}

function mbedtls()
{
    cmd_script="cmake ${opt_toolchain}"
    mbedtls_script="${script_dir}/build_mbedtls.cmake"

    pr_operation_header "Install MBedTLS to ${opt_export}"

    if [[ -z ${opt_export} || -z ${opt_src} || -z ${opt_out} ]]; then
        usage_mbedtls
        exit 1
    fi

    cmd_script="${cmd_script} -DMbedTLS_SRC_PATH=${opt_src}"
    cmd_script="${cmd_script} -DMbedTLS_BUILD_DIR=${opt_out}"
    cmd_script="${cmd_script} -DMbedTLS_ROOT=${opt_export} -P ${mbedtls_script}"

    if [[ ${opt_branch} && ${opt_version} ]]; then
        cmd_script="${cmd_script} -DMbedTLS_BRANCH=${opt_branch}"
        cmd_script="${cmd_script} -DMBedTLS_REVISION=${opt_version}"
    fi

    printf "Execute %s\n" "${cmd_script}"
    eval "${cmd_script}"
}

function smw()
{
    pr_operation_header "Install SMW to ${opt_export}"

    cmd_conf_script="./scripts/smw_configure.sh"
    cmd_build_script="./scripts/smw_build.sh"

    if [[ -z ${opt_out} || -z ${opt_src} || -z ${opt_export} ]]; then
        usage_smw
        exit 1
    fi
    # Save current directory
    local original_dir
    original_dir="$(pwd)"

    # Change to the source directory
    printf "Changing directory to $opt_src\n"
    cd "${opt_src}" || { echo "Failed to change directory to ${opt_src}"; exit 1; }

    cmd_conf_script="${cmd_conf_script} ${opt_out}"
    cmd_conf_script="${cmd_conf_script} aarch64 ${opt_platform}"
    cmd_conf_script="${cmd_conf_script} toolpath=${opt_toolpath_raw}"

    cmd_build_script="${cmd_build_script} install out=${opt_out}"
    cmd_build_script="${cmd_build_script} dest=${opt_export}"

    printf "Execute %s\n" "${cmd_conf_script}"
    eval "${cmd_conf_script}"
 
    printf "Execute %s\n" "${cmd_build_script}"
    eval "${cmd_build_script}"

    # Return to the original directory
    cd "${original_dir}" || { echo "Failed to return to original directory"; exit 1; }
}

function el2go_agent()
{
    cmd_script="cmake ${opt_toolchain}"
    mbedtls_script="${script_dir}/build_el2go_agent.cmake"

    pr_operation_header "Install EdgeLock 2GO Agent to ${opt_export}"

    if [[ -z ${opt_export} || -z ${opt_src} || -z ${opt_smw} || -z ${opt_mbedtls} || -z ${opt_out} ]]; then
        usage_el2go_agent
        exit 1
    fi

    cmd_script="${cmd_script} -Del2go_agent_SRC_PATH=${opt_src}"
    cmd_script="${cmd_script} -DNXP_SMW_DIR=${opt_smw}"
    cmd_script="${cmd_script} -Del2go_agent_BUILD_DIR=${opt_out}"
    cmd_script="${cmd_script} -DMbedTLS_DIR=${opt_mbedtls}"
    cmd_script="${cmd_script} -Del2go_agent_ROOT=${opt_export} -P ${mbedtls_script}"

    printf "Execute %s\n" "${cmd_script}"
    eval "${cmd_script}"
}

function configure()
{
    pr_operation_header "Configure oem-prov-app to ${opt_out}"

    if [[ -z ${opt_out} ]]; then
        usage_configure
        exit 1
    fi

    # Set default verbose level if not specified
    if [[ -z ${opt_verbose} ]]; then
        opt_verbose="-DVERBOSE=2"
    fi

    export PKG_CONFIG_PATH="${opt_cyaml}"
    cmd_script="cmake -S . -B ${opt_out} ${opt_toolchain}"
    cmd_script="${cmd_script} ${opt_buildtype} ${opt_verbose} ${opt_test}"
    if [[ -n ${opt_smw} ]]; then
        cmd_script="${cmd_script} -DNXP_SMW_DIR=${opt_smw}"
    fi
    if [[ -n ${opt_el2go_agent} ]]; then
        cmd_script="${cmd_script} -Del2go_agent_DIR=${opt_el2go_agent}"
    fi
    printf "Execute %s\n" "${cmd_script}"
    eval "${cmd_script}"
}

function build()
{
    pr_operation_header "Build oem-prov-app (${opt_build}) to ${opt_out}"

    if [[ -z ${opt_out} ]]; then
        usage_build
        exit 1
    fi

    cmd_make="make -C ${opt_out}"
    printf "Execute %s\n" "${cmd_make}"
    eval "${cmd_make}"
}

function install()
{
    pr_operation_header "Install oem-prov-app to ${opt_dest}"

    if [[ -z ${opt_out} ]]; then
        usage_install
        exit 1
    fi

    cmd_make="make -C ${opt_out}"

    cmd_script=""
    if [[ -n ${opt_dest} ]]; then
        cmd_script="DESTDIR=${opt_dest}"
    fi
    printf "Execute ${cmd_make} install ${cmd_script}\n"
    eval "${cmd_make} install ${cmd_script}"
}

check_cmake_version

if [[ $# -eq 0 ]]; then
    usage
fi

opt_action="$1"
shift

for arg in "$@"
do
    case ${arg} in
        toolpath=*)
            opt_toolpath_raw="${arg#*=}"
            check_directory opt_toolpath_raw
            opt_toolpath="-DTOOLCHAIN_PATH=${opt_toolpath_raw}"
            ;;
        toolname=*)
            opt_toolname="${arg#*=}"
            opt_toolname="-DTOOLCHAIN_NAME=${opt_toolname}"
            ;;
        export=*)
            opt_export="${arg#*=}"
            check_directory opt_export
            ;;
        src=*)
            opt_src="${arg#*=}"
            check_directory opt_src
            ;;
        smw=*)
            opt_smw="${arg#*=}"
            check_directory opt_smw
            ;;
        mbedtls=*)
            opt_mbedtls="${arg#*=}"
            check_directory opt_mbedtls
            ;;
        el2go_agent=*)
            opt_el2go_agent="${arg#*=}"
            check_directory opt_el2go_agent
            ;;
        platform=*)
            opt_platform="${arg#*=}"
            ;;
        out=*)
            opt_out="${arg#*=}"
            opt_builddir="-DBUILD_DIR=${opt_out}"
            ;;
        dest=*)
            opt_dest="${arg#*=}"
            ;;
        cyaml=*)
            opt_cyaml="${arg#*=}"
            ;;
        debug)
            opt_buildtype="-DCMAKE_BUILD_TYPE=Debug"
            ;;
        verbose=*)
            opt_verbose="${arg#*=}"
            opt_verbose="-DVERBOSE=${opt_verbose}"
            ;;
        version=*)
            opt_version="${arg#*=}"
            ;;
        branch=*)
            opt_branch="${arg#*=}"
            ;;
        hash=*)
            opt_hash="${arg#*=}"
            ;;
# configure option
        test)
            opt_test="-DCLOSE_COMMIT_DRY_RUN=ON"
            ;;
        *)
            pr_err "Unknown argument \"${arg}\""
            usage
            ;;
    esac

    shift
done

toolchain_script="${script_dir}/aarch64_toolchain.cmake"
if [[ ! -e "$toolchain_script" ]]; then
    pr_err "Toolchain script is missing: ${toolchain_script}"
    usage
fi
opt_toolscript="-DCMAKE_TOOLCHAIN_FILE=${toolchain_script}"
opt_toolchain="${opt_toolname} ${opt_toolpath} ${opt_toolscript}"

case ${opt_action} in
    toolchain)
        toolchain
        ;;
    configure)
        configure
        ;;
    yaml)
       yaml
       ;;
    cyaml)
       cyaml
       ;;
    mbedtls)
        mbedtls
        ;;
    smw)
        smw
        ;;
    el2go_agent)
       el2go_agent
       ;;
    build)
        build
        ;;
    install)
        install
        ;;
    *)
        usage
        ;;
esac
