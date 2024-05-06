"use strict";(self.webpackChunktinyorm_org=self.webpackChunktinyorm_org||[]).push([[755],{7667:(e,s,n)=>{n.r(s),n.d(s,{assets:()=>o,contentTitle:()=>t,default:()=>h,frontMatter:()=>r,metadata:()=>a,toc:()=>l});var d=n(4848),i=n(8453);const r={sidebar_position:3,sidebar_label:"Seeding",description:"TinyORM includes the ability to seed your database with data using seed classes. The DatabaseSeeder class is considered as the root seeder. From this class, you may use the `call` method to run other seed classes, allowing you to control the seeding order.",keywords:["c++ orm","database","seeder","seeding","tinyorm"]},t="Database: Seeding",a={id:"database/seeding",title:"Database: Seeding",description:"TinyORM includes the ability to seed your database with data using seed classes. The DatabaseSeeder class is considered as the root seeder. From this class, you may use the `call` method to run other seed classes, allowing you to control the seeding order.",source:"@site/docs/database/seeding.mdx",sourceDirName:"database",slug:"/database/seeding",permalink:"/database/seeding",draft:!1,unlisted:!1,tags:[],version:"current",sidebarPosition:3,frontMatter:{sidebar_position:3,sidebar_label:"Seeding",description:"TinyORM includes the ability to seed your database with data using seed classes. The DatabaseSeeder class is considered as the root seeder. From this class, you may use the `call` method to run other seed classes, allowing you to control the seeding order.",keywords:["c++ orm","database","seeder","seeding","tinyorm"]},sidebar:"tinyormSidebar",previous:{title:"Migrations",permalink:"/database/migrations"},next:{title:"Getting Started",permalink:"/tinyorm/getting-started"}},o={},l=[{value:"Introduction",id:"introduction",level:2},{value:"Writing Seeders",id:"writing-seeders",level:2},{value:"Calling Additional Seeders",id:"calling-additional-seeders",level:3},{value:"Call with additional arguments",id:"call-with-additional-arguments",level:4},{value:"Running Seeders",id:"running-seeders",level:2},{value:"Forcing Seeders To Run In Production",id:"forcing-seeders-to-run-in-production",level:4}];function c(e){const s={a:"a",admonition:"admonition",code:"code",h1:"h1",h2:"h2",h3:"h3",h4:"h4",li:"li",p:"p",pre:"pre",ul:"ul",...(0,i.R)(),...e.components};return(0,d.jsxs)(d.Fragment,{children:[(0,d.jsx)(s.h1,{id:"database-seeding",children:"Database: Seeding"}),"\n",(0,d.jsxs)(s.ul,{children:["\n",(0,d.jsx)(s.li,{children:(0,d.jsx)(s.a,{href:"#introduction",children:"Introduction"})}),"\n",(0,d.jsxs)(s.li,{children:[(0,d.jsx)(s.a,{href:"#writing-seeders",children:"Writing Seeders"}),"\n",(0,d.jsxs)(s.ul,{children:["\n",(0,d.jsx)(s.li,{children:(0,d.jsx)(s.a,{href:"#calling-additional-seeders",children:"Calling Additional Seeders"})}),"\n"]}),"\n"]}),"\n",(0,d.jsx)(s.li,{children:(0,d.jsx)(s.a,{href:"#running-seeders",children:"Running Seeders"})}),"\n"]}),"\n",(0,d.jsx)(s.h2,{id:"introduction",children:"Introduction"}),"\n",(0,d.jsxs)(s.p,{children:["TinyORM includes the ability to seed your database with data using seed classes. All seed classes should be stored in the ",(0,d.jsx)(s.code,{children:"database/seeders"})," directory. The ",(0,d.jsx)(s.code,{children:"DatabaseSeeder"})," class is considered as the root seeder. From this class, you may use the ",(0,d.jsx)(s.code,{children:"call"})," method to run other seed classes, allowing you to control the seeding order."]}),"\n",(0,d.jsx)(s.admonition,{type:"tip",children:(0,d.jsxs)(s.p,{children:[(0,d.jsx)(s.a,{href:"/tinyorm/getting-started#mass-assignment",children:"Mass assignment protection"})," is automatically disabled during database seeding."]})}),"\n",(0,d.jsx)(s.h2,{id:"writing-seeders",children:"Writing Seeders"}),"\n",(0,d.jsxs)(s.p,{children:["To generate a seeder, execute the ",(0,d.jsx)(s.code,{children:"make:seeder"})," ",(0,d.jsx)(s.code,{children:"tom"})," command. A new seeder will be placed in the ",(0,d.jsx)(s.code,{children:"database/seeders"})," directory relative to the current ",(0,d.jsx)("abbr",{title:"Current working directory",children:"pwd"}),":"]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-bash",children:"tom make:seeder UserSeeder\n"})}),"\n",(0,d.jsx)(s.admonition,{type:"tip",children:(0,d.jsxs)(s.p,{children:["You can omit the ",(0,d.jsx)(s.code,{children:"Seeder"})," word in the class name, ",(0,d.jsx)(s.code,{children:"tom"})," appends it for you."]})}),"\n",(0,d.jsxs)(s.p,{children:["A seeder class only contains one method by default: ",(0,d.jsx)(s.code,{children:"run"}),". This method is called when the ",(0,d.jsx)(s.code,{children:"db:seed"})," tom command is executed. Within the ",(0,d.jsx)(s.code,{children:"run"})," method, you may insert data into your database however you wish. You may use the ",(0,d.jsx)(s.a,{href:"/database/query-builder#insert-statements",children:"query builder"})," to manually insert data."]}),"\n",(0,d.jsxs)(s.p,{children:["As an example, let's modify the default ",(0,d.jsx)(s.code,{children:"DatabaseSeeder"})," class and add a database insert statement to the ",(0,d.jsx)(s.code,{children:"run"})," method:"]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-cpp",children:'#pragma once\n\n#include <tom/seeder.hpp>\n\nnamespace Seeders\n{\n\n    /*! Main database seeder. */\n    struct DatabaseSeeder : Seeder\n    {\n        /*! Run the database seeders. */\n        void run() override\n        {\n            DB::table("users")->insert({"name", "email"},\n            {\n                {"1. user", "user1@example.com"},\n                {"2. user", "user2@example.com"},\n            });\n        }\n    };\n\n} // namespace Seeders\n'})}),"\n",(0,d.jsx)(s.admonition,{type:"tip",children:(0,d.jsxs)(s.p,{children:["The multi-insert ",(0,d.jsx)(s.a,{href:"/database/query-builder#multi-insert-overload",children:(0,d.jsx)(s.code,{children:"insert"})})," method overload is ideal for seeding data."]})}),"\n",(0,d.jsx)(s.h3,{id:"calling-additional-seeders",children:"Calling Additional Seeders"}),"\n",(0,d.jsxs)(s.p,{children:["Within the ",(0,d.jsx)(s.code,{children:"DatabaseSeeder"})," class, you may use the ",(0,d.jsx)(s.code,{children:"call"})," method to execute additional seed classes. Using the ",(0,d.jsx)(s.code,{children:"call"})," method allows you to break up your database seeding into multiple files so that no single seeder class becomes too large. The ",(0,d.jsx)(s.code,{children:"call"})," method accepts the template parameter pack of seeder classes that should be executed:"]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-cpp",children:"/*! Run the database seeders. */\nvoid run() override\n{\n    call<UserSeeder, PostSeeder, CommentSeeder>();\n}\n"})}),"\n",(0,d.jsx)(s.h4,{id:"call-with-additional-arguments",children:"Call with additional arguments"}),"\n",(0,d.jsxs)(s.p,{children:["The ",(0,d.jsx)(s.code,{children:"call"})," method allows to pass additional arguments to the seeder/s, but it has additional requirements."]}),"\n",(0,d.jsxs)(s.p,{children:["If you define a ",(0,d.jsx)(s.code,{children:"run"})," method without parameters then this method is called using the virtual dispatch (polymorphism) and in this case, you should use the ",(0,d.jsx)(s.code,{children:"override"})," specifier."]}),"\n",(0,d.jsxs)(s.p,{children:["If you define your ",(0,d.jsx)(s.code,{children:"run"})," method eg. like this ",(0,d.jsx)(s.code,{children:"run(bool shouldSeed)"})," or whatever parameters you want, then this method is called using the fold expression (virtual dispatch is not used in this case) so you can't use the ",(0,d.jsx)(s.code,{children:"override"})," specifier and you must call the ",(0,d.jsx)(s.code,{children:"call<>()"})," method with exactly the same arguments like the ",(0,d.jsx)(s.code,{children:"run"})," method was defined with, in our example, it should look like this ",(0,d.jsx)(s.code,{children:"call<ExampleSeeder>(true)"}),"."]}),"\n",(0,d.jsxs)(s.p,{children:["Let's demonstrate it on a small example, following is the ",(0,d.jsx)(s.code,{children:"run"})," method in the root ",(0,d.jsx)(s.code,{children:"DatabaseSeeder"})," class."]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-cpp",children:"/*! Run the database seeders. */\nvoid run() override\n{\n    // This value can be based eg. on data from the database\n    const auto shouldSeed = true;\n\n    call<UserSeeder>(shouldSeed);\n}\n"})}),"\n",(0,d.jsxs)(s.p,{children:["The ",(0,d.jsx)(s.code,{children:"run"})," method in the ",(0,d.jsx)(s.code,{children:"UserSeeder"})," class."]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-cpp",children:'/*! Run the database seeders. */\nvoid run(const bool shouldSeed)\n{\n    if (!shouldSeed)\n        return;\n\n    DB::table("users")->insert({\n        {"name", "1. user"},\n    });\n}\n'})}),"\n",(0,d.jsx)(s.admonition,{type:"tip",children:(0,d.jsxs)(s.p,{children:["The ",(0,d.jsx)(s.code,{children:"call"})," method provides two shortcut methods, ",(0,d.jsx)(s.code,{children:"callWith"})," and ",(0,d.jsx)(s.code,{children:"callSilent"})," (no output from seeders)."]})}),"\n",(0,d.jsx)(s.h2,{id:"running-seeders",children:"Running Seeders"}),"\n",(0,d.jsxs)(s.p,{children:["You may execute the ",(0,d.jsx)(s.code,{children:"db:seed"})," tom command to seed your database. By default, the ",(0,d.jsx)(s.code,{children:"db:seed"})," command runs the ",(0,d.jsx)(s.code,{children:"Seeders::DatabaseSeeder"})," class, which may in turn invoke other seed classes. However, you may use the ",(0,d.jsx)(s.code,{children:"--class"})," option to specify a specific seeder class to run individually:"]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-bash",children:"tom db:seed\n\ntom db:seed --class=UserSeeder\n"})}),"\n",(0,d.jsxs)(s.p,{children:["You may also seed your database using the ",(0,d.jsx)(s.code,{children:"migrate"}),", ",(0,d.jsx)(s.code,{children:"migrate:fresh"})," or ",(0,d.jsx)(s.code,{children:"migrate:refresh"})," commands in combination with the ",(0,d.jsx)(s.code,{children:"--seed"})," option. For example the ",(0,d.jsx)(s.code,{children:"migrate:fresh"})," command drops all tables and re-run all of your migrations. This command is useful for completely re-building your database:"]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-bash",children:"tom migrate:fresh --seed\n"})}),"\n",(0,d.jsx)(s.admonition,{type:"tip",children:(0,d.jsxs)(s.p,{children:["You can change the default seeders path as is described in the ",(0,d.jsx)(s.a,{href:"/building/tinyorm#TINYTOM_SEEDERS_DIR",children:(0,d.jsx)(s.code,{children:"C preprocessor macros"})}),", CMake provides the ",(0,d.jsx)(s.a,{href:"/building/tinyorm#TOM_SEEDERS_DIR",children:(0,d.jsx)(s.code,{children:"TOM_SEEDERS_DIR"})})," option."]})}),"\n",(0,d.jsx)(s.h4,{id:"forcing-seeders-to-run-in-production",children:"Forcing Seeders To Run In Production"}),"\n",(0,d.jsxs)(s.p,{children:["Some seeding operations may cause you to alter or lose data. In order to protect you from running seeding commands against your production database, you will be prompted for confirmation before the seeders are executed in the ",(0,d.jsx)(s.code,{children:"production"})," environment. To force the seeders to run without a prompt, use the ",(0,d.jsx)(s.code,{children:"--force"})," flag:"]}),"\n",(0,d.jsx)(s.pre,{children:(0,d.jsx)(s.code,{className:"language-bash",children:"tom db:seed --force\n"})})]})}function h(e={}){const{wrapper:s}={...(0,i.R)(),...e.components};return s?(0,d.jsx)(s,{...e,children:(0,d.jsx)(c,{...e})}):c(e)}},8453:(e,s,n)=>{n.d(s,{R:()=>t,x:()=>a});var d=n(6540);const i={},r=d.createContext(i);function t(e){const s=d.useContext(r);return d.useMemo((function(){return"function"==typeof e?e(s):{...s,...e}}),[s,e])}function a(e){let s;return s=e.disableParentContext?"function"==typeof e.components?e.components(i):e.components||i:t(e.components),d.createElement(r.Provider,{value:s},e.children)}}}]);