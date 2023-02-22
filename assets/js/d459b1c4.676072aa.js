"use strict";(self.webpackChunktinyorm_org=self.webpackChunktinyorm_org||[]).push([[966],{3905:(e,t,n)=>{n.d(t,{Zo:()=>d,kt:()=>c});var r=n(7294);function i(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function l(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);t&&(r=r.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,r)}return n}function o(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?l(Object(n),!0).forEach((function(t){i(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):l(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function a(e,t){if(null==e)return{};var n,r,i=function(e,t){if(null==e)return{};var n,r,i={},l=Object.keys(e);for(r=0;r<l.length;r++)n=l[r],t.indexOf(n)>=0||(i[n]=e[n]);return i}(e,t);if(Object.getOwnPropertySymbols){var l=Object.getOwnPropertySymbols(e);for(r=0;r<l.length;r++)n=l[r],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(i[n]=e[n])}return i}var p=r.createContext({}),s=function(e){var t=r.useContext(p),n=t;return e&&(n="function"==typeof e?e(t):o(o({},t),e)),n},d=function(e){var t=s(e.components);return r.createElement(p.Provider,{value:t},e.children)},m={inlineCode:"code",wrapper:function(e){var t=e.children;return r.createElement(r.Fragment,{},t)}},u=r.forwardRef((function(e,t){var n=e.components,i=e.mdxType,l=e.originalType,p=e.parentName,d=a(e,["components","mdxType","originalType","parentName"]),u=s(n),c=i,k=u["".concat(p,".").concat(c)]||u[c]||m[c]||l;return n?r.createElement(k,o(o({ref:t},d),{},{components:n})):r.createElement(k,o({ref:t},d))}));function c(e,t){var n=arguments,i=t&&t.mdxType;if("string"==typeof e||i){var l=n.length,o=new Array(l);o[0]=u;var a={};for(var p in t)hasOwnProperty.call(t,p)&&(a[p]=t[p]);a.originalType=e,a.mdxType="string"==typeof e?e:i,o[1]=a;for(var s=2;s<l;s++)o[s]=n[s];return r.createElement.apply(null,o)}return r.createElement.apply(null,n)}u.displayName="MDXCreateElement"},4742:(e,t,n)=>{n.r(t),n.d(t,{assets:()=>p,contentTitle:()=>o,default:()=>m,frontMatter:()=>l,metadata:()=>a,toc:()=>s});var r=n(7462),i=(n(7294),n(3905));const l={sidebar_position:2,sidebar_label:"\ud83d\ude80 Supported Compilers",hide_table_of_contents:!0,description:"Platform requirements, supported compilers and build systems for TinyORM c++ library.",keywords:["c++ orm","supported compilers","supported build systems","tinyorm"]},o="Supported Compilers",a={unversionedId:"supported-compilers",id:"supported-compilers",title:"Supported Compilers",description:"Platform requirements, supported compilers and build systems for TinyORM c++ library.",source:"@site/docs/supported-compilers.mdx",sourceDirName:".",slug:"/supported-compilers",permalink:"/supported-compilers",draft:!1,editUrl:"https://github.com/silverqx/TinyORM-github.io/edit/main/docs/supported-compilers.mdx",tags:[],version:"current",sidebarPosition:2,frontMatter:{sidebar_position:2,sidebar_label:"\ud83d\ude80 Supported Compilers",hide_table_of_contents:!0,description:"Platform requirements, supported compilers and build systems for TinyORM c++ library.",keywords:["c++ orm","supported compilers","supported build systems","tinyorm"]},sidebar:"tinyormSidebar",previous:{title:"\ud83d\udd27 Dependencies",permalink:"/dependencies"},next:{title:"Getting Started",permalink:"/database/getting-started"}},p={},s=[{value:"<code>Windows &gt;=10</code>",id:"windows-10",level:4},{value:"<code>Linux</code>",id:"linux",level:4},{value:"Supported build systems",id:"supported-build-systems",level:2},{value:"Make tools",id:"make-tools",level:5},{value:"Parallel building",id:"parallel-building",level:5}],d={toc:s};function m(e){let{components:t,...n}=e;return(0,i.kt)("wrapper",(0,r.Z)({},d,n,{components:t,mdxType:"MDXLayout"}),(0,i.kt)("h1",{id:"supported-compilers"},"Supported Compilers"),(0,i.kt)("p",null,"Following compilers are backed up by the GitHub Action ",(0,i.kt)("a",{parentName:"p",href:"https://github.com/silverqx/TinyORM/tree/main/.github/workflows"},"workflows")," (CI pipelines), these workflows also include more then ",(0,i.kt)("strong",{parentName:"p"},"2186 unit tests")," \ud83d\ude2e\ud83d\udca5."),(0,i.kt)("div",{id:"supported-compilers"},(0,i.kt)("h4",{id:"windows-10"},(0,i.kt)("inlineCode",{parentName:"h4"},"Windows >=10")),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},"MSVC 2019 ",(0,i.kt)("inlineCode",{parentName:"li"},">=16.9")),(0,i.kt)("li",{parentName:"ul"},"MSVC 2022 ",(0,i.kt)("inlineCode",{parentName:"li"},">=17")),(0,i.kt)("li",{parentName:"ul"},"MSYS2 UCRT64 GCC ",(0,i.kt)("inlineCode",{parentName:"li"},"10.2")," - ",(0,i.kt)("inlineCode",{parentName:"li"},"12.2")),(0,i.kt)("li",{parentName:"ul"},"MSYS2 UCRT64 Clang ",(0,i.kt)("inlineCode",{parentName:"li"},"12")," - ",(0,i.kt)("inlineCode",{parentName:"li"},"15")),(0,i.kt)("li",{parentName:"ul"},"clang-cl ",(0,i.kt)("inlineCode",{parentName:"li"},">=14")," with MSVC 2019/2022")),(0,i.kt)("h4",{id:"linux"},(0,i.kt)("inlineCode",{parentName:"h4"},"Linux")),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},"GCC ",(0,i.kt)("inlineCode",{parentName:"li"},"10.2")," - ",(0,i.kt)("inlineCode",{parentName:"li"},"12.1")),(0,i.kt)("li",{parentName:"ul"},"Clang ",(0,i.kt)("inlineCode",{parentName:"li"},"11")," - ",(0,i.kt)("inlineCode",{parentName:"li"},"14")))),(0,i.kt)("admonition",{type:"tip"},(0,i.kt)("p",{parentName:"admonition"},"You can compile TinyORM with the MSVC 2022 even if Qt doesn't provide binaries for the MSVC 2022, you can link against Qt MSVC 2019 binaries without any limitations.")),(0,i.kt)("h2",{id:"supported-build-systems"},"Supported build systems"),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("inlineCode",{parentName:"li"},"CMake")," ",(0,i.kt)("inlineCode",{parentName:"li"},">=3.22")," ",(0,i.kt)("small",{className:"darker"},"(policies <= CMP0128 default to NEW)")),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("inlineCode",{parentName:"li"},"qmake")," distributed by the Qt Framework")),(0,i.kt)("h5",{id:"make-tools"},"Make tools"),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("inlineCode",{parentName:"li"},"jom")," - highly recommended with the ",(0,i.kt)("inlineCode",{parentName:"li"},"qmake")," build system on Windows ",(0,i.kt)("small",{className:"darker"},"(replacement for nmake)")),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("inlineCode",{parentName:"li"},"ninja")," - recommended for ",(0,i.kt)("inlineCode",{parentName:"li"},"CMake")," as the make file generator")),(0,i.kt)("h5",{id:"parallel-building"},"Parallel building"),(0,i.kt)("p",null,"You can control parallel building using the following environment variables."),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},"CMake - ",(0,i.kt)("inlineCode",{parentName:"li"},"CMAKE_BUILD_PARALLEL_LEVEL")," eg. to ",(0,i.kt)("inlineCode",{parentName:"li"},"10")),(0,i.kt)("li",{parentName:"ul"},"jom - ",(0,i.kt)("inlineCode",{parentName:"li"},"JOMFLAGS")," eg. to ",(0,i.kt)("inlineCode",{parentName:"li"},"j11")),(0,i.kt)("li",{parentName:"ul"},"vcpkg - ",(0,i.kt)("inlineCode",{parentName:"li"},"VCPKG_MAX_CONCURRENCY")," eg. to ",(0,i.kt)("inlineCode",{parentName:"li"},"10"))))}m.isMDXComponent=!0}}]);