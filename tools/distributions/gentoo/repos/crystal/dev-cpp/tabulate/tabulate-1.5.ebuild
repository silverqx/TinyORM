# Copyright 2024 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

inherit cmake

DESCRIPTION="Table Maker for Modern C++"
HOMEPAGE="https://github.com/p-ranav/tabulate"
LICENSE="MIT"
SLOT="0"

if [[ ${PV} == *9999 ]]; then
	inherit git-r3
	EGIT_REPO_URI="https://github.com/p-ranav/tabulate.git"
	EGIT_MIN_CLONE_TYPE="single"
else
	SRC_URI="https://github.com/p-ranav/tabulate/archive/refs/tags/v${PV}.tar.gz"
	KEYWORDS="~amd64"
fi
