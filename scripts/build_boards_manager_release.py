#!/usr/bin/env python3

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import shutil
import subprocess
import sys
import tarfile
import tempfile


PACKAGE_NAME = "ti"
ARCHITECTURE = "mspm0"
BOARD_NAME = "LP-MSPM0C1104"
DEFAULT_GCC_VERSION = "15.2.1-1.1"
DEFAULT_OPENOCD_VERSION = "0.12.0-7"
TOOL_RELEASES = {
    "arm-none-eabi-gcc": {
        "15.2.1-1.1": [
            {
                "host": "x86_64-mingw32",
                "url": "https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v15.2.1-1.1/xpack-arm-none-eabi-gcc-15.2.1-1.1-win32-x64.zip",
                "archiveFileName": "xpack-arm-none-eabi-gcc-15.2.1-1.1-win32-x64.zip",
                "checksum": "SHA-256:bae6a3d1667697ce750c3b13d6d26d80973ecedc2cc87bf04869e83447fd93ea",
                "size": "335571206",
            },
            {
                "host": "i686-mingw32",
                "url": "https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v15.2.1-1.1/xpack-arm-none-eabi-gcc-15.2.1-1.1-win32-x64.zip",
                "archiveFileName": "xpack-arm-none-eabi-gcc-15.2.1-1.1-win32-x64.zip",
                "checksum": "SHA-256:bae6a3d1667697ce750c3b13d6d26d80973ecedc2cc87bf04869e83447fd93ea",
                "size": "335571206",
            },
            {
                "host": "x86_64-pc-linux-gnu",
                "url": "https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v15.2.1-1.1/xpack-arm-none-eabi-gcc-15.2.1-1.1-linux-x64.tar.gz",
                "archiveFileName": "xpack-arm-none-eabi-gcc-15.2.1-1.1-linux-x64.tar.gz",
                "checksum": "SHA-256:da6a49ad4003944b823c6c93702a8787c922ab34bd7e918ec0eaf6933a9b1ff6",
                "size": "307090703",
            },
            {
                "host": "aarch64-linux-gnu",
                "url": "https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v15.2.1-1.1/xpack-arm-none-eabi-gcc-15.2.1-1.1-linux-arm64.tar.gz",
                "archiveFileName": "xpack-arm-none-eabi-gcc-15.2.1-1.1-linux-arm64.tar.gz",
                "checksum": "SHA-256:67980c7990eba7bb7ffdf39699102effd70889f5ac427be19a8c8a6c5fab2972",
                "size": "302624714",
            },
            {
                "host": "arm64-apple-darwin",
                "url": "https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v15.2.1-1.1/xpack-arm-none-eabi-gcc-15.2.1-1.1-darwin-arm64.tar.gz",
                "archiveFileName": "xpack-arm-none-eabi-gcc-15.2.1-1.1-darwin-arm64.tar.gz",
                "checksum": "SHA-256:574082d35e49a2bcbdc355836b2a3ae5e5bb3b9456c9f5e37177db2ab4aad870",
                "size": "293820200",
            },
            {
                "host": "x86_64-apple-darwin",
                "url": "https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v15.2.1-1.1/xpack-arm-none-eabi-gcc-15.2.1-1.1-darwin-x64.tar.gz",
                "archiveFileName": "xpack-arm-none-eabi-gcc-15.2.1-1.1-darwin-x64.tar.gz",
                "checksum": "SHA-256:5be906a5194c3173e145a58048e5607ffff947773237802b93e55e23c415f1b6",
                "size": "298312329",
            },
        ]
    },
    "openocd": {
        "0.12.0-7": [
            {
                "host": "x86_64-mingw32",
                "url": "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-win32-x64.zip",
                "archiveFileName": "xpack-openocd-0.12.0-7-win32-x64.zip",
                "checksum": "SHA-256:6bfd3c97135aafef8affc9af1acf34fd0e2b9ca26044506f6abd7f95b7630052",
                "size": "3225998",
            },
            {
                "host": "i686-mingw32",
                "url": "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-win32-x64.zip",
                "archiveFileName": "xpack-openocd-0.12.0-7-win32-x64.zip",
                "checksum": "SHA-256:6bfd3c97135aafef8affc9af1acf34fd0e2b9ca26044506f6abd7f95b7630052",
                "size": "3225998",
            },
            {
                "host": "x86_64-pc-linux-gnu",
                "url": "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-linux-x64.tar.gz",
                "archiveFileName": "xpack-openocd-0.12.0-7-linux-x64.tar.gz",
                "checksum": "SHA-256:94b3790983beaf8ed57e646c0620dd66d705fddae03d290823a6ed3b439468d6",
                "size": "2802056",
            },
            {
                "host": "aarch64-linux-gnu",
                "url": "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-linux-arm64.tar.gz",
                "archiveFileName": "xpack-openocd-0.12.0-7-linux-arm64.tar.gz",
                "checksum": "SHA-256:db73a3ab91c556ecec2405a7e02d404b11139df6aba1031cad94a7e6766d06cc",
                "size": "2736149",
            },
            {
                "host": "arm64-apple-darwin",
                "url": "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-darwin-arm64.tar.gz",
                "archiveFileName": "xpack-openocd-0.12.0-7-darwin-arm64.tar.gz",
                "checksum": "SHA-256:667342c086984f3e5a55b4e0d5f711add13fb04de040fca493303000e6c19327",
                "size": "2385815",
            },
            {
                "host": "x86_64-apple-darwin",
                "url": "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-7/xpack-openocd-0.12.0-7-darwin-x64.tar.gz",
                "archiveFileName": "xpack-openocd-0.12.0-7-darwin-x64.tar.gz",
                "checksum": "SHA-256:668ad25350103a4357e11629ec833eae5982e973889ce25bad0c2963e37fa8bf",
                "size": "2481762",
            },
        ]
    },
}
ROOT_ITEMS = (
    "ArduinoCore-API/LICENSE",
    "ArduinoCore-API/README.md",
    "ArduinoCore-API/api",
    "LICENSE",
    "README.md",
    "README_ja.md",
    "boards.txt",
    "cores",
    "examples",
    "libraries",
    "mspm0-sdk/license_mspm0_sdk_2_10_00_04.txt",
    "mspm0-sdk/source/third_party/CMSIS/Core/Include",
    "mspm0-sdk/source/ti/devices",
    "mspm0-sdk/source/ti/driverlib",
    "platform.txt",
    "programmers.txt",
    "variants",
)
EXCLUDED_NAMES = {
    ".DS_Store",
    ".claude",
    ".clangd",
    ".git",
    ".github",
    ".gitmodules",
    ".idea",
    ".meta",
    ".metadata",
    ".vscode",
    "__pycache__",
    "build",
    "dist",
    "lib",
    "linker_files",
    "out",
    "startup_system_files",
}
EXCLUDED_SUFFIXES = {
    ".bin",
    ".elf",
    ".hex",
    ".map",
    ".pyc",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build a Boards Manager archive and package index."
    )
    parser.add_argument(
        "--repo-root",
        type=pathlib.Path,
        default=pathlib.Path(__file__).resolve().parents[1],
        help="Repository root to package.",
    )
    parser.add_argument(
        "--output-dir",
        type=pathlib.Path,
        default=None,
        help="Directory for the generated archive and package index.",
    )
    parser.add_argument(
        "--index-path",
        type=pathlib.Path,
        default=None,
        help="Path for the generated package index JSON.",
    )
    parser.add_argument(
        "--version",
        default=None,
        help="Platform version. Defaults to the version from platform.txt.",
    )
    parser.add_argument(
        "--archive-url",
        default=None,
        help="Public URL for the archive. Defaults to a file:// URL for local testing.",
    )
    parser.add_argument(
        "--archive-name",
        default=None,
        help="Archive file name. Defaults to core-ti-mspm0-<version>.tar.bz2.",
    )
    parser.add_argument(
        "--website-url",
        default=None,
        help="Package website URL. Defaults to the git origin converted to HTTPS.",
    )
    parser.add_argument(
        "--maintainer",
        default="tako0910",
        help="Maintainer name for the package index.",
    )
    parser.add_argument(
        "--email",
        default=None,
        help="Maintainer email for the package index.",
    )
    parser.add_argument(
        "--gcc-version",
        "--tool-version",
        dest="gcc_version",
        default=DEFAULT_GCC_VERSION,
        help="Arduino arm-none-eabi-gcc tool dependency version.",
    )
    parser.add_argument(
        "--openocd-version",
        default=DEFAULT_OPENOCD_VERSION,
        help="Arduino OpenOCD tool dependency version.",
    )
    return parser.parse_args()


def read_platform_version(repo_root: pathlib.Path) -> str:
    for line in (repo_root / "platform.txt").read_text(encoding="utf-8").splitlines():
        if line.startswith("version="):
            return line.split("=", 1)[1].strip()
    raise RuntimeError("platform.txt does not contain a version= entry")


def git_origin_https(repo_root: pathlib.Path) -> str | None:
    try:
        remote = subprocess.run(
            ["git", "-C", str(repo_root), "remote", "get-url", "origin"],
            check=True,
            capture_output=True,
            text=True,
        ).stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None

    if remote.startswith("git@github.com:"):
        path = remote.removeprefix("git@github.com:")
        if path.endswith(".git"):
            path = path[:-4]
        return f"https://github.com/{path}"

    if remote.startswith("https://github.com/") and remote.endswith(".git"):
        return remote[:-4]

    if remote.startswith("https://"):
        return remote

    return None


def should_exclude(relative_path: pathlib.Path) -> bool:
    parts = relative_path.parts
    for part in parts:
        if part in EXCLUDED_NAMES:
            return True
    return relative_path.suffix in EXCLUDED_SUFFIXES


def copy_item(src: pathlib.Path, dst: pathlib.Path, repo_root: pathlib.Path) -> None:
    relative = src.relative_to(repo_root)
    if should_exclude(relative):
        return

    if src.is_dir():
        dst.mkdir(parents=True, exist_ok=True)
        for child in sorted(src.iterdir()):
            copy_item(child, dst / child.name, repo_root)
        return

    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)


def stage_platform(repo_root: pathlib.Path, staging_dir: pathlib.Path) -> None:
    for item_name in ROOT_ITEMS:
        src = repo_root / item_name
        if not src.exists():
            continue
        copy_item(src, staging_dir / item_name, repo_root)


def create_archive(
    staging_dir: pathlib.Path, archive_path: pathlib.Path, root_dir_name: str
) -> None:
    archive_path.parent.mkdir(parents=True, exist_ok=True)
    with tarfile.open(archive_path, "w:bz2") as archive:
        for item in sorted(staging_dir.iterdir()):
            archive.add(item, arcname=f"{root_dir_name}/{item.name}")


def sha256_digest(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def file_url(path: pathlib.Path) -> str:
    return path.resolve().as_uri()


def resolve_tool_systems(tool_name: str, version: str) -> list[dict[str, str]]:
    versions = TOOL_RELEASES.get(tool_name)
    if versions is None:
        raise RuntimeError(f"Unknown tool metadata requested: {tool_name}")

    systems = versions.get(version)
    if systems is None:
        available = ", ".join(sorted(versions))
        raise RuntimeError(
            f"No metadata registered for {tool_name} {version}. "
            f"Available versions: {available}"
        )

    return systems


def build_index(
    version: str,
    archive_name: str,
    archive_url: str,
    checksum: str,
    size: int,
    website_url: str,
    maintainer: str,
    email: str | None,
    gcc_version: str,
    openocd_version: str,
) -> dict:
    package: dict[str, object] = {
        "name": PACKAGE_NAME,
        "maintainer": maintainer,
        "websiteURL": website_url,
        "help": {"online": website_url},
        "platforms": [
            {
                "name": "Texas Instruments MSPM0 Arduino Core",
                "architecture": ARCHITECTURE,
                "version": version,
                "category": "Contributed",
                "url": archive_url,
                "archiveFileName": archive_name,
                "checksum": f"SHA-256:{checksum}",
                "size": size,
                "boards": [{"name": BOARD_NAME}],
                "toolsDependencies": [
                    {
                        "packager": PACKAGE_NAME,
                        "name": "arm-none-eabi-gcc",
                        "version": gcc_version,
                    },
                    {
                        "packager": PACKAGE_NAME,
                        "name": "openocd",
                        "version": openocd_version,
                    },
                ],
            }
        ],
        "tools": [
            {
                "name": "arm-none-eabi-gcc",
                "version": gcc_version,
                "systems": resolve_tool_systems("arm-none-eabi-gcc", gcc_version),
            },
            {
                "name": "openocd",
                "version": openocd_version,
                "systems": resolve_tool_systems("openocd", openocd_version),
            },
        ],
    }
    if email:
        package["email"] = email

    return {"packages": [package]}


def main() -> int:
    args = parse_args()
    repo_root = args.repo_root.resolve()
    version = args.version or read_platform_version(repo_root)
    output_dir = (
        args.output_dir.resolve()
        if args.output_dir is not None
        else (repo_root / "dist").resolve()
    )
    archive_name = args.archive_name or f"core-ti-mspm0-{version}.tar.bz2"
    archive_path = output_dir / archive_name
    archive_root_name = f"{PACKAGE_NAME}-{ARCHITECTURE}-{version}"
    index_path = (
        args.index_path.resolve()
        if args.index_path is not None
        else (repo_root / "package_ti_mspm0_index.json").resolve()
    )

    website_url = args.website_url or git_origin_https(repo_root)
    if not website_url:
        raise RuntimeError(
            "Could not determine a website URL. Pass --website-url explicitly."
        )

    with tempfile.TemporaryDirectory(prefix="mspm0-package-") as temp_dir_name:
        temp_dir = pathlib.Path(temp_dir_name)
        staging_dir = temp_dir / "platform"
        staging_dir.mkdir()
        stage_platform(repo_root, staging_dir)
        create_archive(staging_dir, archive_path, archive_root_name)

    checksum = sha256_digest(archive_path)
    size = archive_path.stat().st_size
    archive_url = args.archive_url or file_url(archive_path)

    index = build_index(
        version=version,
        archive_name=archive_name,
        archive_url=archive_url,
        checksum=checksum,
        size=size,
        website_url=website_url,
        maintainer=args.maintainer,
        email=args.email,
        gcc_version=args.gcc_version,
        openocd_version=args.openocd_version,
    )

    index_path.parent.mkdir(parents=True, exist_ok=True)
    index_path.write_text(json.dumps(index, indent=2) + "\n", encoding="utf-8")

    print(f"archive: {archive_path}")
    print(f"package index: {index_path}")
    print(f"archive url: {archive_url}")
    print(f"checksum: SHA-256:{checksum}")
    print(f"size: {size}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
