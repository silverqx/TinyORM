"use strict";(self.webpackChunktinyorm_org=self.webpackChunktinyorm_org||[]).push([[467],{348:(e,n,t)=>{t.r(n),t.d(n,{assets:()=>l,contentTitle:()=>a,default:()=>p,frontMatter:()=>d,metadata:()=>o,toc:()=>c});var r=t(4848),i=t(8453),s=t(8774);const d={sidebar_position:1,sidebar_label:"\ud83d\udd27 Dependencies",hide_table_of_contents:!0,description:"Library dependencies are MySQL Connector/C 8, range-v3 >=0.11.0, tabulate and the Qt framework version used during development was 5.15.2 and >=6.2. The code was developed on MSVC 16.9-16.11, MSVC 17.2-17.9, GCC 10.2-14.1, and Clang 11-16.",keywords:["c++ orm","dependencies","tinyorm"]},a="Dependencies",o={id:"dependencies",title:"Dependencies",description:"Library dependencies are MySQL Connector/C 8, range-v3 >=0.11.0, tabulate and the Qt framework version used during development was 5.15.2 and >=6.2. The code was developed on MSVC 16.9-16.11, MSVC 17.2-17.9, GCC 10.2-14.1, and Clang 11-16.",source:"@site/docs/dependencies.mdx",sourceDirName:".",slug:"/dependencies",permalink:"/dependencies",draft:!1,unlisted:!1,tags:[],version:"current",sidebarPosition:1,frontMatter:{sidebar_position:1,sidebar_label:"\ud83d\udd27 Dependencies",hide_table_of_contents:!0,description:"Library dependencies are MySQL Connector/C 8, range-v3 >=0.11.0, tabulate and the Qt framework version used during development was 5.15.2 and >=6.2. The code was developed on MSVC 16.9-16.11, MSVC 17.2-17.9, GCC 10.2-14.1, and Clang 11-16.",keywords:["c++ orm","dependencies","tinyorm"]},sidebar:"tinyormSidebar",previous:{title:"\ud83d\udd25 Prologue",permalink:"/"},next:{title:"\ud83d\ude80 Supported Compilers",permalink:"/supported-compilers"}},l={},c=[{value:"Required",id:"required",level:5},{value:"Optional",id:"optional",level:5},{value:"Install dependencies",id:"install-dependencies",level:3}];function h(e){const n={a:"a",admonition:"admonition",code:"code",del:"del",h1:"h1",h3:"h3",h5:"h5",li:"li",p:"p",pre:"pre",strong:"strong",ul:"ul",...(0,i.R)(),...e.components};return(0,r.jsxs)(r.Fragment,{children:[(0,r.jsx)(n.h1,{id:"dependencies",children:"Dependencies"}),"\n",(0,r.jsxs)(n.p,{children:["The code was developed on MSVC 16.9-16.11, MSVC 17.2-17.9, GCC 10.2-14.1, and Clang 11-18, so may be assumed it will work on future releases of these compilers. Minimum required ISO C++ standard is C++20.\nThe Qt framework version used during development was ",(0,r.jsx)(n.del,{children:"5.15.2"})," and >=6.2."]}),"\n",(0,r.jsx)(n.h5,{id:"required",children:"Required"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsx)(n.li,{children:"minimum ISO C++ standard is C++20"}),"\n",(0,r.jsxs)(n.li,{children:[">= ",(0,r.jsx)(n.a,{href:"https://www.qt.io/download-qt-installer",children:"Qt Framework 6.2"})," - ",(0,r.jsx)(n.a,{href:"https://doc.qt.io/qt-6/qtcore-module.html",children:(0,r.jsx)(n.code,{children:"QtCore"})})," and ",(0,r.jsx)(n.a,{href:"https://doc.qt.io/qt-6/qtsql-index.html",children:(0,r.jsx)(n.code,{children:"QtSql"})})," modules"]}),"\n",(0,r.jsxs)(n.li,{children:[">= ",(0,r.jsx)(n.a,{href:"https://github.com/ericniebler/range-v3",children:"range-v3 0.11.0"})]}),"\n",(0,r.jsx)(n.li,{children:(0,r.jsx)(n.a,{href:"https://github.com/p-ranav/tabulate",children:"tabulate"})}),"\n"]}),"\n",(0,r.jsx)(n.admonition,{type:"warning",children:(0,r.jsxs)(n.p,{children:["Be aware that the standard support for the last release of the ",(0,r.jsx)(n.strong,{children:"Qt v5"})," series ended on ",(0,r.jsx)(n.strong,{children:"26. May 2023"}),". [",(0,r.jsx)(n.a,{href:"https://endoflife.date/qt",children:"1"}),"][",(0,r.jsx)(n.a,{href:"https://www.qt.io/blog/qt-5.15-support-ends",children:"2"}),"]"]})}),"\n",(0,r.jsx)(n.admonition,{type:"danger",children:(0,r.jsxs)(n.p,{children:["Support for ",(0,r.jsx)(n.strong,{children:"Qt v5.15"})," was removed since ",(0,r.jsx)(n.strong,{children:"TinyORM v0.38.0"}),"."]})}),"\n",(0,r.jsx)(n.h5,{id:"optional",children:"Optional"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsxs)(n.li,{children:[">= ",(0,r.jsx)(n.a,{href:"https://dev.mysql.com/downloads/c-api/",children:"MySQL Connector/C 8"})," - used only for the ",(0,r.jsx)(n.a,{href:"https://dev.mysql.com/doc/c-api/8.4/en/mysql-ping.html",children:(0,r.jsx)(n.code,{children:"mysql_ping"})})," function and provided by ",(0,r.jsx)(n.a,{href:"https://dev.mysql.com/downloads/mysql/",children:"MySQL 8 Server"})]}),"\n"]}),"\n",(0,r.jsx)(n.admonition,{type:"info",children:(0,r.jsxs)(n.p,{children:["The ",(0,r.jsx)(n.code,{children:"TinyORM"})," will support ",(0,r.jsx)(n.code,{children:"Qt"})," versions that aren't ",(0,r.jsx)(n.a,{href:"https://endoflife.date/qt",children:"end-of-life"}),"."]})}),"\n",(0,r.jsx)(n.admonition,{type:"note",children:(0,r.jsxs)(n.p,{children:["You can view the supported database servers in the ",(0,r.jsx)(n.a,{href:"/database/getting-started#introduction",children:"Database - Getting Started"})," section."]})}),"\n",(0,r.jsx)(n.h3,{id:"install-dependencies",children:"Install dependencies"}),"\n",(0,r.jsxs)(n.p,{children:["On ",(0,r.jsx)(n.code,{children:"Linux"}),", you can install dependencies with the package manager."]}),"\n",(0,r.jsxs)("small",{children:[(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-text",metastring:"title='MySQL C library'",children:"Arch   - pacman -S mariadb-libs\nGentoo - emerge dev-db/mysql (package.use: -server -perl)\nUbuntu - apt install libmysqlclient-dev\n"})}),(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-text",metastring:"title='range-v3 library (header only)'",children:"Arch   - pacman -S range-v3\nGentoo - emerge dev-cpp/range-v3\nUbuntu - apt install librange-v3-dev\n"})}),(0,r.jsx)(s.A,{id:"linux-installation-ccache"}),(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-text",metastring:"title='ccache'",children:"Arch   - pacman -S ccache\nGentoo - emerge dev-util/ccache\nUbuntu - apt install ccache\n"})})]})]})}function p(e={}){const{wrapper:n}={...(0,i.R)(),...e.components};return n?(0,r.jsx)(n,{...e,children:(0,r.jsx)(h,{...e})}):h(e)}},8453:(e,n,t)=>{t.d(n,{R:()=>d,x:()=>a});var r=t(6540);const i={},s=r.createContext(i);function d(e){const n=r.useContext(s);return r.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function a(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(i):e.components||i:d(e.components),r.createElement(s.Provider,{value:n},e.children)}}}]);