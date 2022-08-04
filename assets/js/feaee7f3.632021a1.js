"use strict";(self.webpackChunktinyorm_org=self.webpackChunktinyorm_org||[]).push([[168],{5162:(e,l,t)=>{t.d(l,{Z:()=>i});var n=t(7294),a=t(6010);const o="tabItem_Ymn6";function i(e){let{children:l,hidden:t,className:i}=e;return n.createElement("div",{role:"tabpanel",className:(0,a.Z)(o,i),hidden:t},l)}},5488:(e,l,t)=>{t.d(l,{Z:()=>c});var n=t(7462),a=t(7294),o=t(6010),i=t(2389),r=t(7392),d=t(7094),p=t(2466);const s="tabList__CuJ",u="tabItem_LNqP";function m(e){var l,t;const{lazy:i,block:m,defaultValue:c,values:k,groupId:h,className:b}=e,g=a.Children.map(e.children,(e=>{if((0,a.isValidElement)(e)&&"value"in e.props)return e;throw new Error("Docusaurus error: Bad <Tabs> child <"+("string"==typeof e.type?e.type:e.type.name)+'>: all children of the <Tabs> component should be <TabItem>, and every <TabItem> should have a unique "value" prop.')})),N=null!=k?k:g.map((e=>{let{props:{value:l,label:t,attributes:n}}=e;return{value:l,label:t,attributes:n}})),C=(0,r.l)(N,((e,l)=>e.value===l.value));if(C.length>0)throw new Error('Docusaurus error: Duplicate values "'+C.map((e=>e.value)).join(", ")+'" found in <Tabs>. Every value needs to be unique.');const T=null===c?c:null!=(l=null!=c?c:null==(t=g.find((e=>e.props.default)))?void 0:t.props.value)?l:g[0].props.value;if(null!==T&&!N.some((e=>e.value===T)))throw new Error('Docusaurus error: The <Tabs> has a defaultValue "'+T+'" but none of its children has the corresponding value. Available values are: '+N.map((e=>e.value)).join(", ")+". If you intend to show no default tab, use defaultValue={null} instead.");const{tabGroupChoices:y,setTabGroupChoices:w}=(0,d.U)(),[f,v]=(0,a.useState)(T),I=[],{blockElementScrollPositionUntilNextRender:E}=(0,p.o5)();if(null!=h){const e=y[h];null!=e&&e!==f&&N.some((l=>l.value===e))&&v(e)}const _=e=>{const l=e.currentTarget,t=I.indexOf(l),n=N[t].value;n!==f&&(E(l),v(n),null!=h&&w(h,String(n)))},H=e=>{var l;let t=null;switch(e.key){case"ArrowRight":{var n;const l=I.indexOf(e.currentTarget)+1;t=null!=(n=I[l])?n:I[0];break}case"ArrowLeft":{var a;const l=I.indexOf(e.currentTarget)-1;t=null!=(a=I[l])?a:I[I.length-1];break}}null==(l=t)||l.focus()};return a.createElement("div",{className:(0,o.Z)("tabs-container",s)},a.createElement("ul",{role:"tablist","aria-orientation":"horizontal",className:(0,o.Z)("tabs",{"tabs--block":m},b)},N.map((e=>{let{value:l,label:t,attributes:i}=e;return a.createElement("li",(0,n.Z)({role:"tab",tabIndex:f===l?0:-1,"aria-selected":f===l,key:l,ref:e=>I.push(e),onKeyDown:H,onFocus:_,onClick:_},i,{className:(0,o.Z)("tabs__item",u,null==i?void 0:i.className,{"tabs__item--active":f===l})}),null!=t?t:l)}))),i?(0,a.cloneElement)(g.filter((e=>e.props.value===f))[0],{className:"margin-top--md"}):a.createElement("div",{className:"margin-top--md"},g.map(((e,l)=>(0,a.cloneElement)(e,{key:l,hidden:e.props.value!==f})))))}function c(e){const l=(0,i.Z)();return a.createElement(m,(0,n.Z)({key:String(l)},e))}},2044:(e,l,t)=>{t.d(l,{Ae:()=>u,C:()=>p,Fo:()=>i,IM:()=>s,IZ:()=>n,RS:()=>k,al:()=>c,jk:()=>d,js:()=>r,q5:()=>o,qb:()=>m,wU:()=>a});const n="shell",a="application",o="bash",i="pwsh",r="zsh",d="application",p="bash",s="pwsh",u="zsh",m="TestApp",c="$HOME/Code/c/",k="$env:USERPROFILE\\Code\\c\\"},4355:(e,l,t)=>{t.d(l,{Z:()=>o});var n=t(7294),a=t(9482);function o(){const e=(0,n.useContext)(a.Z);if(null!=e)return e;throw new Error("useRootFolderContext is used outside of Layout component.")}},6005:(e,l,t)=>{t.d(l,{AE:()=>r,EA:()=>i,em:()=>p,go:()=>d,mT:()=>s,we:()=>u});var n=t(4355),a=t(2389),o=t(2044);const i=function(e,l){var t;return void 0===l&&(l=!0),m(null!=(t=(0,n.Z)().rootFolder[e])?t:p(e),e,l)},r=()=>{var e;return null!=(e=(0,n.Z)().rootFolder[o.wU])?e:p(o.wU)},d=function(e,l){if(void 0===l&&(l=!0),null==e)throw new Error("The groupId in the applicationFolderPath() can not be empty.");const t=l||e!==o.Fo?"/":"\\";return m(i(e)+t+r(),e,l)};function p(e){if(null==e)throw new Error("The groupId in the folderDefaultValue() can not be empty.");if(!(0,a.Z)())return"";switch(e){case o.Fo:return o.RS;case o.q5:return o.al;case o.wU:return o.qb;default:throw new Error("No default value for '"+e+"' groupId in the folderDefaultValue().")}}function s(e){return e===o.wU}function u(e,l){if(null==l||""===l)return l;const t="$ENV{$1}$2";switch(e){case o.Fo:return k(l).replace(/\$env:(.+?)(\/.*)/,t);case o.q5:return l.replace(/\$(.+?)(\/.*)/,t);default:throw new Error("Unsupported shell type '"+e+"' in the convertToCmakeEnvVariable().")}}function m(e,l,t){if(void 0===t&&(t=!0),null==e||""===e)return e;if(l!==o.Fo)return c(e);const n=c(e);return t?k(n):function(e){return null==e||""===e?e:e.replaceAll(/\/+/g,"\\")}(n)}function c(e){return null==e||""===e?e:e.replace(/[/\\]+$/,"")}function k(e){return null==e||""===e?e:e.replaceAll(/\\+(?! )/g,"/")}},4588:(e,l,t)=>{t.r(l),t.d(l,{assets:()=>c,contentTitle:()=>u,default:()=>b,frontMatter:()=>s,metadata:()=>m,toc:()=>k});var n=t(7462),a=(t(7294),t(3905)),o=t(452),i=t(5162),r=t(5488),d=t(2044),p=t(6005);const s={sidebar_position:2,sidebar_label:"Hello world",description:"Hello world example created in the terminal and QtCreator IDE.",keywords:["c++ orm","building","hello world","tinyorm"]},u="Building: Hello world",m={unversionedId:"building/hello-world",id:"building/hello-world",title:"Building: Hello world",description:"Hello world example created in the terminal and QtCreator IDE.",source:"@site/docs/building/hello-world.mdx",sourceDirName:"building",slug:"/building/hello-world",permalink:"/building/hello-world",draft:!1,editUrl:"https://github.com/silverqx/TinyORM-github.io/edit/main/docs/building/hello-world.mdx",tags:[],version:"current",sidebarPosition:2,frontMatter:{sidebar_position:2,sidebar_label:"Hello world",description:"Hello world example created in the terminal and QtCreator IDE.",keywords:["c++ orm","building","hello world","tinyorm"]},sidebar:"tinyormSidebar",previous:{title:"TinyORM",permalink:"/building/tinyorm"},next:{title:"Migrations",permalink:"/building/migrations"}},c={},k=[{value:"Introduction",id:"introduction",level:2},{value:"Prepare SQLite 3 database",id:"prepare-sqlite-3-database",level:2},{value:"Install dependencies",id:"install-dependencies",level:2},{value:"Using vcpkg.json <small>(manifest mode)</small>",id:"using-vcpkg-json",level:4},{value:"Using vcpkg install <small>(manually)</small>",id:"using-vcpkg-install",level:4},{value:"Source code",id:"source-code",level:2},{value:"Hello world with CMake",id:"hello-world-with-cmake",level:2},{value:"CMake project",id:"cmake-project",level:3},{value:"Build Hello world",id:"build-hello-world-cmake",level:3},{value:"Execute Hello world",id:"execute-hello-world-cmake",level:3},{value:"Hello world with qmake",id:"hello-world-with-qmake",level:2},{value:"qmake project",id:"qmake-project",level:3},{value:"Configure using .qmake.conf",id:"configure-using-qmakeconf",level:4},{value:"Build Hello world",id:"build-hello-world-qmake",level:3},{value:"Execute Hello world",id:"execute-hello-world-qmake",level:3}],h={toc:k};function b(e){let{components:l,...s}=e;return(0,a.kt)("wrapper",(0,n.Z)({},h,s,{components:l,mdxType:"MDXLayout"}),(0,a.kt)("h1",{id:"building-hello-world"},"Building: Hello world"),(0,a.kt)("ul",null,(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#introduction"},"Introduction")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#prepare-sqlite-3-database"},"Prepare SQLite 3 database")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#install-dependencies"},"Install dependencies"),(0,a.kt)("ul",{parentName:"li"},(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#using-vcpkg-json"},"Using vcpkg.json")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#using-vcpkg-install"},"Using vcpkg install")))),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#source-code"},"Source code")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#hello-world-with-cmake"},"Hello world with CMake"),(0,a.kt)("ul",{parentName:"li"},(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#cmake-project"},"CMake project")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#build-hello-world-cmake"},"Build Hello world")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#execute-hello-world-cmake"},"Execute Hello world")))),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#hello-world-with-qmake"},"Hello world with qmake"),(0,a.kt)("ul",{parentName:"li"},(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#qmake-project"},"qmake project")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#build-hello-world-qmake"},"Build Hello world")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#execute-hello-world-qmake"},"Execute Hello world"))))),(0,a.kt)("h2",{id:"introduction"},"Introduction"),(0,a.kt)("p",null,"We will try to create the simplest working application in the terminal with the ",(0,a.kt)("inlineCode",{parentName:"p"},"CMake")," and in the ",(0,a.kt)("inlineCode",{parentName:"p"},"QtCreator")," IDE with the ",(0,a.kt)("inlineCode",{parentName:"p"},"qmake"),"."),(0,a.kt)("p",null,"The ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," example also expects the following ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#folders-structure"},"folders structure"),", let's create them."),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-powershell",mdxType:"CodeBlock"},"cd "+(0,p.go)(d.Fo)+"\nmkdir HelloWorld/HelloWorld\ncd HelloWorld")),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-bash",mdxType:"CodeBlock"},"cd "+(0,p.go)(d.q5)+"\nmkdir -p HelloWorld/HelloWorld\ncd HelloWorld"))),(0,a.kt)("h2",{id:"prepare-sqlite-3-database"},"Prepare SQLite 3 database"),(0,a.kt)("p",null,"Simplest will be to demonstrate the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," example with the ",(0,a.kt)("inlineCode",{parentName:"p"},"SQLite 3")," database."),(0,a.kt)("p",null,"To create and insert two rows into the ",(0,a.kt)("inlineCode",{parentName:"p"},"SQLite 3")," database, execute the following command in the terminal."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"sqlite3 HelloWorld.sqlite3 \"\ncreate table posts(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, name VARCHAR NOT NULL);\ninsert into posts values(1, 'First Post');\ninsert into posts values(2, 'Second Post');\nselect * from posts;\"\n")),(0,a.kt)("h2",{id:"install-dependencies"},"Install dependencies"),(0,a.kt)("p",null,"First, install the ",(0,a.kt)("inlineCode",{parentName:"p"},"vcpkg")," package manager as is described ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#vcpkg"},"here"),"."),(0,a.kt)("p",null,"The ",(0,a.kt)("inlineCode",{parentName:"p"},"range-v3")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"tabulate")," libraries are required dependencies because ",(0,a.kt)("inlineCode",{parentName:"p"},"TinyORM")," uses them in header files, you have to install them before you can use ",(0,a.kt)("inlineCode",{parentName:"p"},"TinyORM"),". The ",(0,a.kt)("inlineCode",{parentName:"p"},"tabulate")," library is only needed in the ",(0,a.kt)("inlineCode",{parentName:"p"},"tom")," migrations it's used by the ",(0,a.kt)("inlineCode",{parentName:"p"},"migrate:status")," command."),(0,a.kt)("p",null,"There are two ways how to install the ",(0,a.kt)("inlineCode",{parentName:"p"},"range-v3")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"tabulate")," libraries using ",(0,a.kt)("inlineCode",{parentName:"p"},"vcpkg"),"."),(0,a.kt)("h4",{id:"using-vcpkg-json"},"Using vcpkg.json ",(0,a.kt)("small",null,"(manifest mode)")),(0,a.kt)("p",null,"Create a ",(0,a.kt)("inlineCode",{parentName:"p"},"vcpkg.json")," file with the following content. ",(0,a.kt)("inlineCode",{parentName:"p"},"CMake")," example below uses this method."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"cd HelloWorld\nvim vcpkg.json\n")),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-json"},'{\n  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg/master/scripts/vcpkg.schema.json",\n  "name": "tinyorm-hello-world",\n  "version-semver": "0.1.0",\n  "description": "HelloWorld example with TinyORM library.",\n  "maintainers": "Silver Zachara <silver.zachara@gmail.com>",\n  "supports": "!(uwp | arm | android | emscripten)",\n  "dependencies": [\n    "range-v3",\n    "tabulate"\n  ]\n}\n')),(0,a.kt)("admonition",{type:"note"},(0,a.kt)("p",{parentName:"admonition"},"Only ",(0,a.kt)("inlineCode",{parentName:"p"},"CMake")," via the ",(0,a.kt)("inlineCode",{parentName:"p"},"toolchain file")," supports this method.")),(0,a.kt)("h4",{id:"using-vcpkg-install"},"Using vcpkg install ",(0,a.kt)("small",null,"(manually)")),(0,a.kt)("p",null,"This method can be used with both ",(0,a.kt)("inlineCode",{parentName:"p"},"CMake")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"qmake"),"."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"cd ../../vcpkg\n\nvcpkg search range-v3\nvcpkg search tabulate\nvcpkg install range-v3 tabulate\nvcpkg list\n")),(0,a.kt)("h2",{id:"source-code"},"Source code"),(0,a.kt)("p",null,"Let's start in the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," project folder."),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-powershell",mdxType:"CodeBlock"},"cd "+(0,p.go)(d.Fo)+"/HelloWorld/HelloWorld")),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-bash",mdxType:"CodeBlock"},"cd "+(0,p.go)(d.q5)+"/HelloWorld/HelloWorld"))),(0,a.kt)("p",null,"Create ",(0,a.kt)("inlineCode",{parentName:"p"},"main.cpp")," source file."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"vim main.cpp\n")),(0,a.kt)("admonition",{type:"tip"},(0,a.kt)("p",{parentName:"admonition"},"To paste a source code correctly in ",(0,a.kt)("inlineCode",{parentName:"p"},"vim"),", press ",(0,a.kt)("kbd",null,"Shift")," + ",(0,a.kt)("kbd",null,"p"),".")),(0,a.kt)("p",null,"And paste the following code."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'#include <QDebug>\n\n#ifdef _WIN32\n#  include <qt_windows.h>\n#endif\n\n#include <orm/db.hpp>\n\nusing Orm::DB;\n\nint main(int /*unused*/, char */*unused*/[])\n{\n#ifdef _WIN32\n    SetConsoleOutputCP(CP_UTF8);\n//    SetConsoleOutputCP(1250);\n#endif\n\n    // Ownership of a shared_ptr()\n    auto manager = DB::create({\n        {"driver",   "QSQLITE"},\n        {"database", qEnvironmentVariable("DB_DATABASE", "HelloWorld.sqlite3")},\n        {"check_database_exists", true},\n    });\n\n    auto posts = DB::select("select * from posts");\n\n    while(posts.next())\n        qDebug() << posts.value("id").toULongLong() << posts.value("name").toString();\n}\n')),(0,a.kt)("h2",{id:"hello-world-with-cmake"},"Hello world with CMake"),(0,a.kt)("p",null,"Create a folder for the ",(0,a.kt)("inlineCode",{parentName:"p"},"CMake")," build."),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-powershell",mdxType:"CodeBlock"},"cd ..\nmkdir HelloWorld-builds-cmake/build-debug\n\ncd HelloWorld-builds-cmake/build-debug")),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-bash",mdxType:"CodeBlock"},"cd ..\nmkdir -p HelloWorld-builds-cmake/build-debug\n\ncd HelloWorld-builds-cmake/build-debug"))),(0,a.kt)("h3",{id:"cmake-project"},"CMake project"),(0,a.kt)("p",null,"Create ",(0,a.kt)("inlineCode",{parentName:"p"},"CMakeLists.txt")," file with the following content."),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-cmake",mdxType:"CodeBlock"},'cmake_minimum_required(VERSION VERSION 3.20...3.23 FATAL_ERROR)\n\nproject(HelloWorld LANGUAGES CXX)\n\n# build tree\nlist(APPEND CMAKE_PREFIX_PATH "'+(0,p.we)(d.Fo,(0,p.go)(d.Fo))+'/TinyORM/TinyORM-builds-cmake/build-debug")\n\nset(CMAKE_CXX_STANDARD 20)\nset(CMAKE_CXX_STANDARD_REQUIRED ON)\nset(CMAKE_CXX_EXTENSIONS OFF)\n\nadd_executable(HelloWorld\n  main.cpp\n)\n\nfind_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)\nfind_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)\nfind_package(TinyOrm 0.1.0 CONFIG REQUIRED)\n\ntarget_link_libraries(HelloWorld\n    PRIVATE\n        Qt${QT_VERSION_MAJOR}::Core\n        TinyOrm::TinyOrm\n)')),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-cmake",mdxType:"CodeBlock"},'cmake_minimum_required(VERSION VERSION 3.20...3.23 FATAL_ERROR)\n\nproject(HelloWorld LANGUAGES CXX)\n\n# build tree\nlist(APPEND CMAKE_PREFIX_PATH "'+(0,p.we)(d.q5,(0,p.go)(d.q5))+'/TinyORM/TinyORM-builds-cmake/build-debug")\n\nset(CMAKE_CXX_STANDARD 20)\nset(CMAKE_CXX_STANDARD_REQUIRED ON)\nset(CMAKE_CXX_EXTENSIONS OFF)\n\nadd_executable(HelloWorld\n  main.cpp\n)\n\nfind_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)\nfind_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)\nfind_package(TinyOrm 0.1.0 CONFIG REQUIRED)\n\ntarget_link_libraries(HelloWorld\n    PRIVATE\n        Qt${QT_VERSION_MAJOR}::Core\n        TinyOrm::TinyOrm\n)'))),(0,a.kt)("h3",{id:"build-hello-world-cmake"},"Build Hello world"),(0,a.kt)("p",null,"Now you are ready to configure ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," ",(0,a.kt)("inlineCode",{parentName:"p"},"CMake")," application."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"cd ../HelloWorld-builds-cmake/build-debug\n")),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-powershell",mdxType:"CodeBlock"},'cmake.exe `\n-S "'+(0,p.go)(d.Fo)+'/HelloWorld/HelloWorld" `\n-B "'+(0,p.go)(d.Fo)+"/HelloWorld/HelloWorld-builds-cmake/build-debug\" `\n-G 'Ninja' `\n-D CMAKE_BUILD_TYPE:STRING='Debug' `\n-D CMAKE_TOOLCHAIN_FILE:FILEPATH=\""+(0,p.EA)(d.Fo)+'/vcpkg/scripts/buildsystems/vcpkg.cmake" `\n-D CMAKE_INSTALL_PREFIX:PATH="'+(0,p.EA)(d.Fo)+'/tmp/HelloWorld"')),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-bash",mdxType:"CodeBlock"},'cmake \\\n-S "'+(0,p.go)(d.q5)+'/HelloWorld/HelloWorld" \\\n-B "'+(0,p.go)(d.q5)+"/HelloWorld/HelloWorld-builds-cmake/build-debug\" \\\n-G 'Ninja' \\\n-D CMAKE_BUILD_TYPE:STRING='Debug' \\\n-D CMAKE_TOOLCHAIN_FILE:FILEPATH=\""+(0,p.EA)(d.q5)+'/vcpkg/scripts/buildsystems/vcpkg.cmake" \\\n-D CMAKE_INSTALL_PREFIX:PATH="'+(0,p.EA)(d.q5)+'/tmp/TinyORM"'))),(0,a.kt)("p",null,"And build."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"cmake --build . --target all\n")),(0,a.kt)("h3",{id:"execute-hello-world-cmake"},"Execute Hello world"),(0,a.kt)("p",null,"Do not forget to add ",(0,a.kt)("inlineCode",{parentName:"p"},"TinyOrm0d.dll")," on the path on Windows and on the ",(0,a.kt)("inlineCode",{parentName:"p"},"LD_LIBRARY_PATH")," on Linux, so ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," application can find it during execution, as is described ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#tinyorm-on-path-cmake"},"here"),"."),(0,a.kt)(r.Z,{groupId:d.IZ,name:"tinyorm-on-path",mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-powershell",mdxType:"CodeBlock"},'$env:Path = "'+(0,p.go)(d.Fo,!1)+'\\TinyORM\\TinyORM-builds-cmake\\build-debug;" + $env:Path')),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-bash",mdxType:"CodeBlock"},"export LD_LIBRARY_PATH="+(0,p.go)(d.q5)+"/TinyORM/TinyORM-builds-cmake/build-debug${PATH:+:}$PATH"))),(0,a.kt)("p",null,"Create a symbolic link to the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld.sqlite3")," database inside the build folder. If you do not have enabled symbolic links without ",(0,a.kt)("inlineCode",{parentName:"p"},"Administrator")," rights on your ",(0,a.kt)("inlineCode",{parentName:"p"},"Windows"),", you can just simply copy the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld.sqlite3")," database or ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#allow-symbolic-links-unprivileged"},(0,a.kt)("inlineCode",{parentName:"a"},"Allow symbolic links unprivileged")),"."),(0,a.kt)(r.Z,{groupId:d.IZ,name:"tinyorm-on-path",mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-powershell"},"New-Item -ItemType SymbolicLink -Target ../../HelloWorld.sqlite3 -Name HelloWorld.sqlite3\n# Or simply copy\nCopy-Item ../../HelloWorld.sqlite3 .\n"))),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"ln -s ../../HelloWorld.sqlite3 .\n")))),(0,a.kt)("p",null,"Execute ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," example."),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-powershell"},".\\HelloWorld.exe\n"))),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"./HelloWorld\n")))),(0,a.kt)("p",null,"The output will look like this."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-less"},'Executed prepared query (6ms, -1 results, 0 affected, tinyorm_default) : select * from posts\n1 "First Post"\n2 "Second Post"\n')),(0,a.kt)("h2",{id:"hello-world-with-qmake"},"Hello world with qmake"),(0,a.kt)("p",null,"Create a folder for the ",(0,a.kt)("inlineCode",{parentName:"p"},"qmake")," build."),(0,a.kt)(r.Z,{groupId:d.IZ,mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-powershell",mdxType:"CodeBlock"},"cd "+(0,p.go)(d.Fo)+"/HelloWorld\n\nmkdir HelloWorld-builds-qmake")),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)(o.Z,{className:"language-bash",mdxType:"CodeBlock"},"cd "+(0,p.go)(d.q5)+"/HelloWorld\n\nmkdir HelloWorld-builds-qmake"))),(0,a.kt)("p",null,"The ",(0,a.kt)("a",{parentName:"p",href:"#source-code"},(0,a.kt)("inlineCode",{parentName:"a"},"source code"))," is the same as for the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld CMake")," example."),(0,a.kt)("h3",{id:"qmake-project"},"qmake project"),(0,a.kt)("p",null,"Create ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld.pro")," qmake file with the following content."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"cd HelloWorld\nvim HelloWorld.pro\n")),(0,a.kt)("admonition",{type:"tip"},(0,a.kt)("p",{parentName:"admonition"},"To paste a source code correctly in ",(0,a.kt)("inlineCode",{parentName:"p"},"vim"),", press ",(0,a.kt)("kbd",null,"Shift")," + ",(0,a.kt)("kbd",null,"p"),".")),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-qmake"},"QT *= core sql\nQT -= gui\n\nTEMPLATE = app\n\nSOURCES += $$PWD/main.cpp\n\n# Configure TinyORM library\ninclude($$TINY_MAIN_DIR/TinyORM/qmake/TinyOrm.pri)\n\n# vcpkg - range-v3\nwin32-msvc: \\\n    INCLUDEPATH += $$quote($$TINY_VCPKG_INSTALLED/x64-windows/include/)\nmingw: \\\n    QMAKE_CXXFLAGS += -isystem $$shell_quote($$TINY_VCPKG_INSTALLED/x64-mingw-dynamic/include/)\nunix:!macx: \\\n    QMAKE_CXXFLAGS += -isystem $$shell_quote($$TINY_VCPKG_INSTALLED/x64-linux/include/)\n")),(0,a.kt)("admonition",{type:"caution"},(0,a.kt)("p",{parentName:"admonition"},"The exact ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#folders-structure"},"folders structure")," is crucial in this example because the paths to the ",(0,a.kt)("inlineCode",{parentName:"p"},"TinyORM")," source and build folders are relative.")),(0,a.kt)("admonition",{type:"tip"},(0,a.kt)("p",{parentName:"admonition"},"On Linux ",(0,a.kt)("inlineCode",{parentName:"p"},"-isystem")," marks the directory as a system directory, it prevents warnings."),(0,a.kt)("p",{parentName:"admonition"},"On Windows you can use ",(0,a.kt)("inlineCode",{parentName:"p"},"QMAKE_CXXFLAGS_WARN_ON = -external:anglebrackets -external:W0"),", it applies a warning level 0 to the angel bracket includes; ",(0,a.kt)("inlineCode",{parentName:"p"},"#include <file>"),".")),(0,a.kt)("h4",{id:"configure-using-qmakeconf"},"Configure using .qmake.conf"),(0,a.kt)("p",null,"Create ",(0,a.kt)("inlineCode",{parentName:"p"},".qmake.conf")," in the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," project root folder with the following content."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-qmake"},"TINY_MAIN_DIR = $$clean_path($$PWD/../../TinyORM)\n# Name of this qmake variable is crucial\nTINYORM_BUILD_TREE = $$quote($$TINY_MAIN_DIR/TinyOrm-builds-qmake/build-TinyOrm-Desktop_Qt_6_3_1_MSVC2019_64bit-Debug)\n# vcpkg - range-v3\nTINY_VCPKG_INSTALLED = $$clean_path($$PWD/../../../vcpkg/installed)\n")),(0,a.kt)("admonition",{type:"info"},(0,a.kt)("p",{parentName:"admonition"},"Configuring with the ",(0,a.kt)("inlineCode",{parentName:"p"},".qmake.conf")," file has one big advantage that is that you do not have to modify the project files.")),(0,a.kt)("h3",{id:"build-hello-world-qmake"},"Build Hello world"),(0,a.kt)("admonition",{type:"tip"},(0,a.kt)("p",{parentName:"admonition"},"I recommend creating a new ",(0,a.kt)("inlineCode",{parentName:"p"},"Session")," in the ",(0,a.kt)("inlineCode",{parentName:"p"},"QtCreator IDE")," as is described ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#open-qtcreator-ide"},"here"),".")),(0,a.kt)("p",null,"Now you can open the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld.pro")," project in the ",(0,a.kt)("inlineCode",{parentName:"p"},"QtCreator IDE"),"."),(0,a.kt)("p",null,"This will open the ",(0,a.kt)("inlineCode",{parentName:"p"},"Configure Project")," tab, select some kit and update build folder paths to meet our ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#folders-structure"},"folders structure")," or like you want."),(0,a.kt)("img",{src:t(1465).Z,alt:"HelloWorld - QtCreator - Configure Project",width:"760"}),(0,a.kt)("p",null,"You are ready to configure build options, hit ",(0,a.kt)("kbd",null,"Ctrl"),"+",(0,a.kt)("kbd",null,"5")," to open ",(0,a.kt)("inlineCode",{parentName:"p"},"Project Settings")," tab and select ",(0,a.kt)("inlineCode",{parentName:"p"},"Build")," in the left sidebar to open the ",(0,a.kt)("inlineCode",{parentName:"p"},"Build Settings"),", it should look similar to the following picture."),(0,a.kt)("img",{src:t(6511).Z,className:"no-blurry",alt:"HelloWorld - QtCreator - Build Settings",width:"760"}),(0,a.kt)("p",null,"Disable ",(0,a.kt)("inlineCode",{parentName:"p"},"QML debugging and profiling")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"Qt Quick Compiler"),", they are not used."),(0,a.kt)("p",null,"In the left sidebar open ",(0,a.kt)("inlineCode",{parentName:"p"},"Dependencies")," and check ",(0,a.kt)("inlineCode",{parentName:"p"},"TinyOrm")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"Synchronize configuration"),", this setting ensures that the current project will be rebuilt correctly when the ",(0,a.kt)("inlineCode",{parentName:"p"},"TinyORM")," library source code changes."),(0,a.kt)("p",null,"Everything is ready to build, you can press ",(0,a.kt)("kbd",null,"Ctrl"),"+",(0,a.kt)("kbd",null,"b")," to build the project."),(0,a.kt)("h3",{id:"execute-hello-world-qmake"},"Execute Hello world"),(0,a.kt)("p",null,"The ",(0,a.kt)("inlineCode",{parentName:"p"},"QtCreator")," takes care about all the necessary configuration, sets up the build environment correctly and also prepends dependency libraries on the path on Windows and on the ",(0,a.kt)("inlineCode",{parentName:"p"},"LD_LIBRARY_PATH")," on Linux."),(0,a.kt)("p",null,"Only one thing you might want to change is to run the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," example in the new terminal window. To do so, hit ",(0,a.kt)("kbd",null,"Ctrl"),"+",(0,a.kt)("kbd",null,"5")," to open the ",(0,a.kt)("inlineCode",{parentName:"p"},"Project Settings")," tab and select ",(0,a.kt)("inlineCode",{parentName:"p"},"Run")," in the left sidebar to open the ",(0,a.kt)("inlineCode",{parentName:"p"},"Run Settings"),", then in the ",(0,a.kt)("inlineCode",{parentName:"p"},"Run")," section select the ",(0,a.kt)("inlineCode",{parentName:"p"},"Run in terminal")," checkbox."),(0,a.kt)("p",null,"Create a symbolic link to the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld.sqlite3")," database inside the build folder. If you do not have enabled symbolic links without ",(0,a.kt)("inlineCode",{parentName:"p"},"Administrator")," rights on your ",(0,a.kt)("inlineCode",{parentName:"p"},"Windows"),", you can just simply copy the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld.sqlite3")," database or ",(0,a.kt)("a",{parentName:"p",href:"/building/tinyorm#allow-symbolic-links-unprivileged"},(0,a.kt)("inlineCode",{parentName:"a"},"Allow symbolic links unprivileged")),"."),(0,a.kt)(r.Z,{groupId:d.IZ,name:"tinyorm-on-path",mdxType:"Tabs"},(0,a.kt)(i.Z,{value:d.Fo,label:d.IM,mdxType:"TabItem"},(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-powershell"},"cd ../HelloWorld-builds-qmake/build-HelloWorld-Desktop_Qt_5_15_2_MSVC2019_64bit-Debug\n\nNew-Item -ItemType SymbolicLink -Target ../../HelloWorld.sqlite3 -Name HelloWorld.sqlite3\n# Or simply copy\nCopy-Item ../../HelloWorld.sqlite3 .\n"))),(0,a.kt)(i.Z,{value:d.q5,label:d.C,mdxType:"TabItem"},(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-bash"},"cd ../HelloWorld-builds-qmake/build-HelloWorld-Desktop_Qt_5_15_2_GCC_64bit-Debug\n\nln -s ../../HelloWorld.sqlite3 .\n")))),(0,a.kt)("p",null,"To execute the ",(0,a.kt)("inlineCode",{parentName:"p"},"HelloWorld")," example press ",(0,a.kt)("kbd",null,"Ctrl")," + ",(0,a.kt)("kbd",null,"r"),"."),(0,a.kt)("p",null,"The output will look like this."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre",className:"language-less"},'Executed prepared query (6ms, -1 results, 0 affected, tinyorm_default) : select * from posts\n1 "First Post"\n2 "Second Post"\n')))}b.isMDXComponent=!0},6511:(e,l,t)=>{t.d(l,{Z:()=>n});const n=t.p+"assets/images/qmake-build_settings-e88e3fd0ef7457887c24004628a7c253.png"},1465:(e,l,t)=>{t.d(l,{Z:()=>n});const n=t.p+"assets/images/qmake-configure_project-7aea3b08d102839d858f5a1b4a22d55b.png"}}]);