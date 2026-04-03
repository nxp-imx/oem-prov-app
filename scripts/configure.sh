#!/bin/bash
set -e

function pr_err()
{
    printf "\033[1;31m\n"
    printf "%s" "$@"
    printf "\033[0m\n"
}

function pr_success()
{
    printf "\033[0;32m\n"
    printf "%s" "$@"
    printf "\033[0m\n"
}

function check_directory()
{
    declare -n mydir=$1
    mydir="${mydir/\~/$HOME}"

    if [[ ! -d "${mydir}" ]]; then
        pr_err "${mydir} is not a directory"
    fi
}

function usage()
{
    cat << EOF

    **********************************************************
    * Usage of OEM Provisioning Application configure script *
    **********************************************************

    The script is configuring the OEM Provisioning Application by
    installing the external dependencies.

    CAUTION: This script must be executed from the OEM Provisioning
    Application directory. The dependency sources should be located
    as sibling directories at the same level as the project folder:

    Expected folder structure:
      parent-folder/
      ├── oem-prov-app/  (run script from here)
      ├── yaml/  (cyaml dependency)
      ├── cyaml/  (oem-prov-app dependency)
      ├── smw/  (oem-prov-app dependency)
      |── optee-client  (smw dependency)
      |── optee-os  (smw dependency)
      |── secure_enclave  (smw dependency)
      └── simw-devicelink/  (oem-prov-app dependency)

    $(basename "$0") <dir> <smw_subsystem> toolpath=[dir]
      <dir>           : Output build directory
      <smw_subsystem> : SMW subsystem(s) - comma-separated list (no spaces)
                        Valid combinations:
                          tee           : TEE Only
                          seco          : SECO Only
                          ele           : ELE Only
                          tee,seco      : SECO + TEE
                          tee,ele       : ELE + TEE
      <toolpath>      : [Optional] Toolchain installation path

EOF
    exit 1
}

if [[ $# -lt 2 ]]; then
    usage
fi

out=$1
smw_subsystem="$2"
shift 2
# change to absolute path to avoid relative path issues
out=$(realpath -m "$out")

readonly smw_out="${out}/smw-build"
readonly smw_export_dir="${out}/export-smw"
readonly export_dir="${out}/export"
readonly export_el2go_agent="${out}/export-el2go-agent"

opt_toolpath="toolpath=/toolchains"
for arg in "$@"
do
    case ${arg} in
        toolpath=*)
            opt_toolpath="${arg#*=}"
            check_directory opt_toolpath
            opt_toolpath="toolpath=${opt_toolpath}"
            ;;
        *)
            pr_err "Unknown argument \"${arg}\""
            usage
            ;;
    esac
done

readonly smw_config="${smw_export_dir}/usr/lib/cmake"
readonly el2go_agent_config="${export_el2go_agent}/usr/lib/cmake/el2go_agent"
readonly cyaml_config="${export_dir}/usr/lib/pkgconfig/"
readonly mbedtls_config="${export_dir}/usr/local/el2go/lib/cmake/MbedTLS"
#
# Build/Prepare external dependencies
#
function build_component() {
    local component="$1"
    shift
    printf "Executing: %s %s\n" "$component" "$*"

    if ! ./scripts/build.sh "$component" "$@"; then
        pr_err "Failed to build ${component}"
        exit 1
    fi
    pr_success "${component} build completed"
}

# Set default source directories
YAML_SRC_DIR="${YAML_SRC_DIR:-../yaml}"
CYAML_SRC_DIR="${CYAML_SRC_DIR:-../cyaml}"
MBEDTLS_SRC_DIR="${MBEDTLS_SRC_DIR:-../mbedtls}"
SMW_SRC_DIR="${SMW_SRC_DIR:-../smw}"
SIMW_DEVICELINK_SRC_DIR="${SIMW_DEVICELINK_SRC_DIR:-../simw-devicelink}"

# toolchain
build_component "toolchain" "${opt_toolpath}"

# yaml library
build_component "yaml" \
    "export=${export_dir}" \
    "src=${YAML_SRC_DIR}" \
    "${opt_toolpath}"

# cyaml library
build_component "cyaml" \
    "export=${export_dir}" \
    "src=${CYAML_SRC_DIR}" \
    "${opt_toolpath}"

# MbedTLS
build_component "mbedtls" \
    "export=${export_dir}" \
    "src=${MBEDTLS_SRC_DIR}" \
    "out=${out}" \
    "${opt_toolpath}"

# SMW
build_component "smw" \
    "export=${smw_export_dir}" \
    "out=${smw_out}" \
    "src=${SMW_SRC_DIR}" \
    "subsystem=${smw_subsystem}" \
    "${opt_toolpath}"

# EdgeLock 2GO Agent
build_component "el2go_agent" \
    "export=${export_el2go_agent}" \
    "smw=${smw_config}" \
    "mbedtls=${mbedtls_config}" \
    "out=${out}" \
    "src=${SIMW_DEVICELINK_SRC_DIR}" \
    "${opt_toolpath}"

# Build configuration options
conf_opts_array=(
    "${opt_toolpath}"
    "smw=${smw_config}"
    "cyaml=${cyaml_config}"
    "el2go_agent=${el2go_agent_config}"
)

# Configure build targets
build_component "configure" \
    "out=${out}" \
    "${conf_opts_array[@]}"
