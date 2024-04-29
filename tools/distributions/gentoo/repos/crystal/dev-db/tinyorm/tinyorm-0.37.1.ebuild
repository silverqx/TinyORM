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

IUSE="build-drivers disable-thread-local inline-constants mysql mysql-ping +orm postgres -qt5 +sqlite +tom +tom-cli"
REQUIRED_USE="
	build-drivers? ( || ( mysql ) !postgres !sqlite !qt5 )
	tom-cli? ( tom )
"

RDEPEND="
	build-drivers? (
		>=dev-qt/qtbase-6.7:6[icu]
		mysql? ( dev-db/mysql-connector-c:= )
	)
	!build-drivers? (
		!qt5? (
			>=dev-qt/qtbase-6.7:6[icu]
			dev-qt/qtbase:=[mysql,postgres,sqlite]
		)
		qt5? (
			dev-qt/qtcore:5[icu]
			dev-qt/qtsql:5[mysql,postgres,sqlite]
		)
	)
	mysql-ping? ( dev-db/mysql-connector-c:= )
"
DEPEND="
	${RDEPEND}
	>=dev-cpp/range-v3-0.12.0
	>=dev-cpp/tabulate-1.5"

DOCS=()

src_configure() {
	local mycmakeargs=(
		-D CMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF
		-D CMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
		-D BUILD_TREE_DEPLOY:BOOL=OFF
		-D STRICT_MODE:BOOL=OFF
		-D VERBOSE_CONFIGURE:BOOL=ON
		-D BUILD_TESTS:BOOL=OFF
		-D BUILD_DRIVERS:BOOL=$(usex build-drivers)
		-D DRIVERS_TYPE:STRING=Shared
		-D DISABLE_THREAD_LOCAL:BOOL=$(usex disable-thread-local)
		-D INLINE_CONSTANTS:BOOL=$(usex inline-constants)
		-D MYSQL_PING:BOOL=$(usex mysql-ping)
		-D ORM:BOOL=$(usex orm)
		-D TOM:BOOL=$(usex tom)
		-D TOM_EXAMPLE:BOOL=$(usex tom-cli)
	)

	use build-drivers && mycmakeargs+=(
		-D BUILD_MYSQL_DRIVER:BOOL=$(usex mysql)
	)

	cmake_src_configure
}
