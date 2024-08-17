"use strict";(self.webpackChunktinyorm_org=self.webpackChunktinyorm_org||[]).push([[27],{9120:(e,i,n)=>{n.r(i),n.d(i,{assets:()=>t,contentTitle:()=>d,default:()=>h,frontMatter:()=>l,metadata:()=>o,toc:()=>c});var s=n(4848),r=n(8453);const l={sidebar_position:2,sidebar_label:"\ud83d\ude80 Supported Compilers",hide_table_of_contents:!0,description:"Platform requirements, supported compilers and build systems for TinyORM C++ library.",keywords:["c++ orm","supported compilers","supported build systems","tinyorm"]},d="Supported Compilers",o={id:"supported-compilers",title:"Supported Compilers",description:"Platform requirements, supported compilers and build systems for TinyORM C++ library.",source:"@site/docs/supported-compilers.mdx",sourceDirName:".",slug:"/supported-compilers",permalink:"/supported-compilers",draft:!1,unlisted:!1,tags:[],version:"current",sidebarPosition:2,frontMatter:{sidebar_position:2,sidebar_label:"\ud83d\ude80 Supported Compilers",hide_table_of_contents:!0,description:"Platform requirements, supported compilers and build systems for TinyORM C++ library.",keywords:["c++ orm","supported compilers","supported build systems","tinyorm"]},sidebar:"tinyormSidebar",previous:{title:"\ud83d\udd27 Dependencies",permalink:"/dependencies"},next:{title:"Getting Started",permalink:"/database/getting-started"}},t={},c=[{value:"<code>Windows &gt;=10</code>",id:"windows-10",level:4},{value:"<code>Linux</code>",id:"linux",level:4},{value:"Supported build systems",id:"supported-build-systems",level:2},{value:"Make tools",id:"make-tools",level:5},{value:"Parallel building",id:"parallel-building",level:5}];function a(e){const i={a:"a",admonition:"admonition",code:"code",h1:"h1",h2:"h2",h4:"h4",h5:"h5",header:"header",li:"li",p:"p",strong:"strong",ul:"ul",...(0,r.R)(),...e.components};return(0,s.jsxs)(s.Fragment,{children:[(0,s.jsx)(i.header,{children:(0,s.jsx)(i.h1,{id:"supported-compilers",children:"Supported Compilers"})}),"\n",(0,s.jsxs)(i.p,{children:["Following compilers are backed up by the GitHub Action ",(0,s.jsx)(i.a,{href:"https://github.com/silverqx/TinyORM/tree/main/.github/workflows",children:"workflows"})," (CI pipelines), these workflows also include more then ",(0,s.jsx)(i.strong,{children:"3378 unit tests"})," \ud83d\ude2e\ud83d\udca5."]}),"\n",(0,s.jsxs)("div",{id:"supported-compilers",children:[(0,s.jsx)(i.h4,{id:"windows-10",children:(0,s.jsx)(i.code,{children:"Windows >=10"})}),(0,s.jsxs)(i.ul,{children:["\n",(0,s.jsxs)(i.li,{children:["MSVC 2019 ",(0,s.jsx)(i.code,{children:">=16.9"})]}),"\n",(0,s.jsxs)(i.li,{children:["MSVC 2022 ",(0,s.jsx)(i.code,{children:">=17"})]}),"\n",(0,s.jsxs)(i.li,{children:["MSYS2 UCRT64 GCC ",(0,s.jsx)(i.code,{children:"11.2"})," - ",(0,s.jsx)(i.code,{children:"14.2"})]}),"\n",(0,s.jsxs)(i.li,{children:["MSYS2 UCRT64 Clang ",(0,s.jsx)(i.code,{children:">=15"})]}),"\n",(0,s.jsxs)(i.li,{children:["Clang-cl ",(0,s.jsx)(i.code,{children:">=15"})," with MSVC 2019/2022"]}),"\n"]}),(0,s.jsx)(i.h4,{id:"linux",children:(0,s.jsx)(i.code,{children:"Linux"})}),(0,s.jsxs)(i.ul,{children:["\n",(0,s.jsxs)(i.li,{children:["GCC ",(0,s.jsx)(i.code,{children:"11.2"})," - ",(0,s.jsx)(i.code,{children:"14.2"})]}),"\n",(0,s.jsxs)(i.li,{children:["Clang ",(0,s.jsx)(i.code,{children:">=15"})," ",(0,s.jsx)("small",{className:"darker",children:"(libstdc++ only)"})]}),"\n"]})]}),"\n",(0,s.jsx)(i.admonition,{type:"tip",children:(0,s.jsx)(i.p,{children:"You can compile TinyORM with the MSVC 2022 even if Qt doesn't provide binaries for the MSVC 2022, you can link against Qt MSVC 2019 binaries without any limitations."})}),"\n",(0,s.jsx)(i.admonition,{type:"info",children:(0,s.jsxs)(i.p,{children:["Qt finally provides ",(0,s.jsx)(i.strong,{children:"MSVC 2022"})," binaries from ",(0,s.jsx)(i.strong,{children:"Qt v6.8"})," (after 2.5 years \ud83e\udd14), what are good news. There will be no MSVC 2019 binaries from Qt v6.8. Everything will work normally and ",(0,s.jsx)(i.strong,{children:"no explicit actions"})," are needed."]})}),"\n",(0,s.jsx)(i.admonition,{type:"warning",children:(0,s.jsxs)(i.p,{children:["The ",(0,s.jsx)(i.code,{children:"macOS"})," and Clang with ",(0,s.jsx)(i.code,{children:"libc++"})," are not supported."]})}),"\n",(0,s.jsx)(i.h2,{id:"supported-build-systems",children:"Supported build systems"}),"\n",(0,s.jsxs)(i.ul,{children:["\n",(0,s.jsxs)(i.li,{children:[(0,s.jsx)(i.code,{children:"CMake"})," ",(0,s.jsx)(i.code,{children:">=3.22"})," ",(0,s.jsx)("small",{className:"darker",children:"(policies <= CMP0128 default to NEW)"})]}),"\n",(0,s.jsxs)(i.li,{children:[(0,s.jsx)(i.code,{children:"qmake"})," distributed by the Qt Framework"]}),"\n"]}),"\n",(0,s.jsx)(i.h5,{id:"make-tools",children:"Make tools"}),"\n",(0,s.jsxs)(i.ul,{children:["\n",(0,s.jsxs)(i.li,{children:[(0,s.jsx)(i.code,{children:"jom"})," - highly recommended with the ",(0,s.jsx)(i.code,{children:"qmake"})," build system on Windows ",(0,s.jsx)("small",{className:"darker",children:"(replacement for nmake)"})]}),"\n",(0,s.jsxs)(i.li,{children:[(0,s.jsx)(i.code,{children:"ninja"})," - recommended for ",(0,s.jsx)(i.code,{children:"CMake"})," as the make file generator"]}),"\n"]}),"\n",(0,s.jsx)(i.h5,{id:"parallel-building",children:"Parallel building"}),"\n",(0,s.jsx)(i.p,{children:"You can control parallel building using the following environment variables."}),"\n",(0,s.jsxs)(i.ul,{children:["\n",(0,s.jsxs)(i.li,{children:["CMake - ",(0,s.jsx)(i.code,{children:"CMAKE_BUILD_PARALLEL_LEVEL"})," eg. to ",(0,s.jsx)(i.code,{children:"10"})]}),"\n",(0,s.jsxs)(i.li,{children:["jom - ",(0,s.jsx)(i.code,{children:"JOMFLAGS"})," eg. to ",(0,s.jsx)(i.code,{children:"j11"})]}),"\n",(0,s.jsxs)(i.li,{children:["make - ",(0,s.jsx)(i.code,{children:"MAKEFLAGS"})," eg. to ",(0,s.jsx)(i.code,{children:"-j10"})]}),"\n",(0,s.jsxs)(i.li,{children:["vcpkg - ",(0,s.jsx)(i.code,{children:"VCPKG_MAX_CONCURRENCY"})," eg. to ",(0,s.jsx)(i.code,{children:"10"})]}),"\n"]})]})}function h(e={}){const{wrapper:i}={...(0,r.R)(),...e.components};return i?(0,s.jsx)(i,{...e,children:(0,s.jsx)(a,{...e})}):a(e)}},8453:(e,i,n)=>{n.d(i,{R:()=>d,x:()=>o});var s=n(6540);const r={},l=s.createContext(r);function d(e){const i=s.useContext(l);return s.useMemo((function(){return"function"==typeof e?e(i):{...i,...e}}),[i,e])}function o(e){let i;return i=e.disableParentContext?"function"==typeof e.components?e.components(r):e.components||r:d(e.components),s.createElement(l.Provider,{value:i},e.children)}}}]);