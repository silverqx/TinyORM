# Copyright 2024 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

inherit cmake

DESCRIPTION="Modern C++ ORM library for Qt framework"
HOMEPAGE="https://www.tinyorm.org https://github.com/silverqx/TinyORM"
LICENSE="MIT"
SLOT="0"

if [[ ${PV} == *9999 ]]; then
	inherit git-r3
	EGIT_REPO_URI="https://github.com/silverqx/TinyORM.git"
	EGIT_MIN_CLONE_TYPE="single"
else
	SRC_URI="https://github.com/silverqx/TinyORM/archive/refs/tags/v${PV}.tar.gz -> ${P}.tar.gz"
	S=${WORKDIR}/TinyORM-${PV}
	KEYWORDS="~amd64"
fi

IUSE="build-drivers disable-thread-local inline-constants lto mysql mysql-ping +orm postgres +sqlite +tom +tom-cli"
REQUIRED_USE="
	build-drivers? ( || ( mysql ) !postgres !sqlite )
	tom-cli? ( tom )
"

RDEPEND="
	>=dev-qt/qtbase-6.7:6[icu]
	build-drivers? (
		mysql? ( dev-db/mysql-connector-c:= )
	)
	!build-drivers? ( dev-qt/qtbase:=[mysql,postgres,sqlite] )
	mysql-ping? ( dev-db/mysql-connector-c:= )
"
DEPEND="
	${RDEPEND}
	>=dev-cpp/range-v3-0.12.0
	>=dev-cpp/tabulate-1.5
"

DOCS=()

src_configure() {
	local mycmakeargs=(
		--log-level=DEBUG --log-context
		-D CMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF
		-D CMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
		-D BUILD_TREE_DEPLOY:BOOL=OFF
    	-D LEAN_HEADERS:BOOL=ON
		-D STRICT_MODE:BOOL=OFF
		-D VERBOSE_CONFIGURE:BOOL=ON
		-D BUILD_TESTS:BOOL=OFF
		-D BUILD_DRIVERS:BOOL=$(usex build-drivers)
		-D DISABLE_THREAD_LOCAL:BOOL=$(usex disable-thread-local)
		-D INLINE_CONSTANTS:BOOL=$(usex inline-constants)
		-D MYSQL_PING:BOOL=$(usex mysql-ping)
		-D ORM:BOOL=$(usex orm)
		-D TOM:BOOL=$(usex tom)
		-D TOM_EXAMPLE:BOOL=$(usex tom-cli)
	)

	use build-drivers && mycmakeargs+=(
		# Change it to loadable when I have more SQL drivers
		-D DRIVERS_TYPE:STRING=Shared
		-D BUILD_MYSQL_DRIVER:BOOL=$(usex mysql)
	)

	use lto && mycmakeargs+=(
	    -D CMAKE_INTERPROCEDURAL_OPTIMIZATION:BOOL=ON
	)

	cmake_src_configure
}
