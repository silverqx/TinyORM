"use strict";(self.webpackChunktinyorm_github_io=self.webpackChunktinyorm_github_io||[]).push([[308],{3905:function(e,t,n){n.d(t,{Zo:function(){return u},kt:function(){return m}});var a=n(7294);function r(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function i(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);t&&(a=a.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,a)}return n}function o(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?i(Object(n),!0).forEach((function(t){r(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):i(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function s(e,t){if(null==e)return{};var n,a,r=function(e,t){if(null==e)return{};var n,a,r={},i=Object.keys(e);for(a=0;a<i.length;a++)n=i[a],t.indexOf(n)>=0||(r[n]=e[n]);return r}(e,t);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(a=0;a<i.length;a++)n=i[a],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(r[n]=e[n])}return r}var d=a.createContext({}),l=function(e){var t=a.useContext(d),n=t;return e&&(n="function"==typeof e?e(t):o(o({},t),e)),n},u=function(e){var t=l(e.components);return a.createElement(d.Provider,{value:t},e.children)},p={inlineCode:"code",wrapper:function(e){var t=e.children;return a.createElement(a.Fragment,{},t)}},c=a.forwardRef((function(e,t){var n=e.components,r=e.mdxType,i=e.originalType,d=e.parentName,u=s(e,["components","mdxType","originalType","parentName"]),c=l(n),m=r,h=c["".concat(d,".").concat(m)]||c[m]||p[m]||i;return n?a.createElement(h,o(o({ref:t},u),{},{components:n})):a.createElement(h,o({ref:t},u))}));function m(e,t){var n=arguments,r=t&&t.mdxType;if("string"==typeof e||r){var i=n.length,o=new Array(i);o[0]=c;var s={};for(var d in t)hasOwnProperty.call(t,d)&&(s[d]=t[d]);s.originalType=e,s.mdxType="string"==typeof e?e:r,o[1]=s;for(var l=2;l<i;l++)o[l]=n[l];return a.createElement.apply(null,o)}return a.createElement.apply(null,n)}c.displayName="MDXCreateElement"},1744:function(e,t,n){n.r(t),n.d(t,{assets:function(){return u},contentTitle:function(){return d},default:function(){return m},frontMatter:function(){return s},metadata:function(){return l},toc:function(){return p}});var a=n(7462),r=n(3366),i=(n(7294),n(3905)),o=["components"],s={sidebar_position:6,description:"Migrations are like version control for your database, allowing your team to define and share the application's database schema definition. Migrations use the Schema facade that provides database agnostic support for creating and manipulating tables across all of TinyORM's supported database systems."},d="Database: Seeding",l={unversionedId:"seeding",id:"seeding",title:"Database: Seeding",description:"Migrations are like version control for your database, allowing your team to define and share the application's database schema definition. Migrations use the Schema facade that provides database agnostic support for creating and manipulating tables across all of TinyORM's supported database systems.",source:"@site/docs/seeding.mdx",sourceDirName:".",slug:"/seeding",permalink:"/seeding",draft:!1,editUrl:"https://github.com/silverqx/TinyORM-github.io/edit/main/docs/seeding.mdx",tags:[],version:"current",sidebarPosition:6,frontMatter:{sidebar_position:6,description:"Migrations are like version control for your database, allowing your team to define and share the application's database schema definition. Migrations use the Schema facade that provides database agnostic support for creating and manipulating tables across all of TinyORM's supported database systems."},sidebar:"tinyormSidebar",previous:{title:"Database: Migrations",permalink:"/migrations"},next:{title:"TinyORM: Getting Started",permalink:"/tinyorm"}},u={},p=[{value:"Introduction",id:"introduction",level:2},{value:"Writing Seeders",id:"writing-seeders",level:2},{value:"Calling Additional Seeders",id:"calling-additional-seeders",level:3},{value:"Running Seeders",id:"running-seeders",level:2},{value:"Forcing Seeders To Run In Production",id:"forcing-seeders-to-run-in-production",level:4}],c={toc:p};function m(e){var t=e.components,n=(0,r.Z)(e,o);return(0,i.kt)("wrapper",(0,a.Z)({},c,n,{components:t,mdxType:"MDXLayout"}),(0,i.kt)("h1",{id:"database-seeding"},"Database: Seeding"),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"#introduction"},"Introduction")),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"#writing-seeders"},"Writing Seeders"),(0,i.kt)("ul",{parentName:"li"},(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"#calling-additional-seeders"},"Calling Additional Seeders")))),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"#running-seeders"},"Running Seeders"))),(0,i.kt)("h2",{id:"introduction"},"Introduction"),(0,i.kt)("p",null,"TinyORM includes the ability to seed your database with data using seed classes. All seed classes should be stored in the ",(0,i.kt)("inlineCode",{parentName:"p"},"database/seeders")," directory. The ",(0,i.kt)("inlineCode",{parentName:"p"},"DatabaseSeeder")," class is considered as the root seeder. From this class, you may use the ",(0,i.kt)("inlineCode",{parentName:"p"},"call")," method to run other seed classes, allowing you to control the seeding order."),(0,i.kt)("div",{className:"admonition admonition-tip alert alert--success"},(0,i.kt)("div",{parentName:"div",className:"admonition-heading"},(0,i.kt)("h5",{parentName:"div"},(0,i.kt)("span",{parentName:"h5",className:"admonition-icon"},(0,i.kt)("svg",{parentName:"span",xmlns:"http://www.w3.org/2000/svg",width:"12",height:"16",viewBox:"0 0 12 16"},(0,i.kt)("path",{parentName:"svg",fillRule:"evenodd",d:"M6.5 0C3.48 0 1 2.19 1 5c0 .92.55 2.25 1 3 1.34 2.25 1.78 2.78 2 4v1h5v-1c.22-1.22.66-1.75 2-4 .45-.75 1-2.08 1-3 0-2.81-2.48-5-5.5-5zm3.64 7.48c-.25.44-.47.8-.67 1.11-.86 1.41-1.25 2.06-1.45 3.23-.02.05-.02.11-.02.17H5c0-.06 0-.13-.02-.17-.2-1.17-.59-1.83-1.45-3.23-.2-.31-.42-.67-.67-1.11C2.44 6.78 2 5.65 2 5c0-2.2 2.02-4 4.5-4 1.22 0 2.36.42 3.22 1.19C10.55 2.94 11 3.94 11 5c0 .66-.44 1.78-.86 2.48zM4 14h5c-.23 1.14-1.3 2-2.5 2s-2.27-.86-2.5-2z"}))),"tip")),(0,i.kt)("div",{parentName:"div",className:"admonition-content"},(0,i.kt)("p",{parentName:"div"},(0,i.kt)("a",{parentName:"p",href:"/tinyorm#mass-assignment"},"Mass assignment protection")," is automatically disabled during database seeding."))),(0,i.kt)("h2",{id:"writing-seeders"},"Writing Seeders"),(0,i.kt)("p",null,"A seeder class only contains one method by default: ",(0,i.kt)("inlineCode",{parentName:"p"},"run"),". This method is called when the ",(0,i.kt)("inlineCode",{parentName:"p"},"db:seed")," tom command is executed. Within the ",(0,i.kt)("inlineCode",{parentName:"p"},"run")," method, you may insert data into your database however you wish. You may use the ",(0,i.kt)("a",{parentName:"p",href:"/query-builder#insert-statements"},"query builder")," to manually insert data."),(0,i.kt)("p",null,"As an example, let's modify the default ",(0,i.kt)("inlineCode",{parentName:"p"},"DatabaseSeeder")," class and add a database insert statement to the ",(0,i.kt)("inlineCode",{parentName:"p"},"run")," method:"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre"},'#pragma once\n\n#include <tom/seeder.hpp>\n\nnamespace Seeders\n{\n\n    /*! Main database seeder. */\n    struct DatabaseSeeder : public Seeder\n    {\n        /*! Run the database seeders. */\n        void run() override\n        {\n            DB::table("users")->insert({\n                {{"name", "1. user"}, {"email", "user1@example.com"}},\n                {{"name", "2. user"}, {"email", "user2@example.com"}},\n            });\n        }\n    };\n\n} // namespace Seeders\n')),(0,i.kt)("h3",{id:"calling-additional-seeders"},"Calling Additional Seeders"),(0,i.kt)("p",null,"Within the ",(0,i.kt)("inlineCode",{parentName:"p"},"DatabaseSeeder")," class, you may use the ",(0,i.kt)("inlineCode",{parentName:"p"},"call")," method to execute additional seed classes. Using the ",(0,i.kt)("inlineCode",{parentName:"p"},"call")," method allows you to break up your database seeding into multiple files so that no single seeder class becomes too large. The ",(0,i.kt)("inlineCode",{parentName:"p"},"call")," method accepts the template parameter pack of seeder classes that should be executed:"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre"},"/*! Run the database seeders. */\nvoid run() override\n{\n    call<UserSeeder, PostSeeder, CommentSeeder>();\n}\n")),(0,i.kt)("h2",{id:"running-seeders"},"Running Seeders"),(0,i.kt)("p",null,"You may execute the ",(0,i.kt)("inlineCode",{parentName:"p"},"db:seed")," tom command to seed your database. By default, the ",(0,i.kt)("inlineCode",{parentName:"p"},"db:seed")," command runs the ",(0,i.kt)("inlineCode",{parentName:"p"},"Seeders::DatabaseSeeder")," class, which may in turn invoke other seed classes. However, you may use the ",(0,i.kt)("inlineCode",{parentName:"p"},"--class")," option to specify a specific seeder class to run individually:"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-bash"},"tom db:seed\n\ntom db:seed --class=UserSeeder\n")),(0,i.kt)("p",null,"You may also seed your database using the ",(0,i.kt)("inlineCode",{parentName:"p"},"migrate"),", ",(0,i.kt)("inlineCode",{parentName:"p"},"migrate:fresh")," or ",(0,i.kt)("inlineCode",{parentName:"p"},"migrate:refresh")," commands in combination with the ",(0,i.kt)("inlineCode",{parentName:"p"},"--seed")," option. For example the ",(0,i.kt)("inlineCode",{parentName:"p"},"migrate:fresh")," command drops all tables and re-run all of your migrations. This command is useful for completely re-building your database:"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-bash"},"tom migrate:fresh --seed\n")),(0,i.kt)("h4",{id:"forcing-seeders-to-run-in-production"},"Forcing Seeders To Run In Production"),(0,i.kt)("p",null,"Some seeding operations may cause you to alter or lose data. In order to protect you from running seeding commands against your production database, you will be prompted for confirmation before the seeders are executed in the ",(0,i.kt)("inlineCode",{parentName:"p"},"production")," environment. To force the seeders to run without a prompt, use the ",(0,i.kt)("inlineCode",{parentName:"p"},"--force")," flag:"),(0,i.kt)("pre",null,(0,i.kt)("code",{parentName:"pre",className:"language-bash"},"tom db:seed --force\n")))}m.isMDXComponent=!0}}]);