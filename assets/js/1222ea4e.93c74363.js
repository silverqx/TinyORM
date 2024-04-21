"use strict";(self.webpackChunktinyorm_org=self.webpackChunktinyorm_org||[]).push([[535],{3905:(e,t,n)=>{n.d(t,{Zo:()=>p,kt:()=>h});var i=n(7294);function a(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function o(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);t&&(i=i.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,i)}return n}function r(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?o(Object(n),!0).forEach((function(t){a(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):o(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function s(e,t){if(null==e)return{};var n,i,a=function(e,t){if(null==e)return{};var n,i,a={},o=Object.keys(e);for(i=0;i<o.length;i++)n=o[i],t.indexOf(n)>=0||(a[n]=e[n]);return a}(e,t);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(i=0;i<o.length;i++)n=o[i],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(a[n]=e[n])}return a}var l=i.createContext({}),d=function(e){var t=i.useContext(l),n=t;return e&&(n="function"==typeof e?e(t):r(r({},t),e)),n},p=function(e){var t=d(e.components);return i.createElement(l.Provider,{value:t},e.children)},u="mdxType",m={inlineCode:"code",wrapper:function(e){var t=e.children;return i.createElement(i.Fragment,{},t)}},c=i.forwardRef((function(e,t){var n=e.components,a=e.mdxType,o=e.originalType,l=e.parentName,p=s(e,["components","mdxType","originalType","parentName"]),u=d(n),c=a,h=u["".concat(l,".").concat(c)]||u[c]||m[c]||o;return n?i.createElement(h,r(r({ref:t},p),{},{components:n})):i.createElement(h,r({ref:t},p))}));function h(e,t){var n=arguments,a=t&&t.mdxType;if("string"==typeof e||a){var o=n.length,r=new Array(o);r[0]=c;var s={};for(var l in t)hasOwnProperty.call(t,l)&&(s[l]=t[l]);s.originalType=e,s[u]="string"==typeof e?e:a,r[1]=s;for(var d=2;d<o;d++)r[d]=n[d];return i.createElement.apply(null,r)}return i.createElement.apply(null,n)}c.displayName="MDXCreateElement"},1412:(e,t,n)=>{n.r(t),n.d(t,{assets:()=>l,contentTitle:()=>r,default:()=>m,frontMatter:()=>o,metadata:()=>s,toc:()=>d});var i=n(7462),a=(n(7294),n(3905));const o={sidebar_position:4,sidebar_label:"Serialization",description:"TinyORM models serialization allows you to serialize models and collection of models including all nested relations to JSON. It also supports converting to vectors or maps and allows controlling a custom date format during serialization.",keywords:["c++ orm","orm","serialization","json","toJson","serializing models","serializing relations","serializing collections","converting","toVector","toMap"]},r="TinyORM: Serialization",s={unversionedId:"tinyorm/serialization",id:"tinyorm/serialization",title:"TinyORM: Serialization",description:"TinyORM models serialization allows you to serialize models and collection of models including all nested relations to JSON. It also supports converting to vectors or maps and allows controlling a custom date format during serialization.",source:"@site/docs/tinyorm/serialization.mdx",sourceDirName:"tinyorm",slug:"/tinyorm/serialization",permalink:"/tinyorm/serialization",draft:!1,editUrl:"https://github.com/silverqx/TinyORM-github.io/edit/main/docs/tinyorm/serialization.mdx",tags:[],version:"current",sidebarPosition:4,frontMatter:{sidebar_position:4,sidebar_label:"Serialization",description:"TinyORM models serialization allows you to serialize models and collection of models including all nested relations to JSON. It also supports converting to vectors or maps and allows controlling a custom date format during serialization.",keywords:["c++ orm","orm","serialization","json","toJson","serializing models","serializing relations","serializing collections","converting","toVector","toMap"]},sidebar:"tinyormSidebar",previous:{title:"Casts",permalink:"/tinyorm/casts"},next:{title:"Getting Started",permalink:"/tinydrivers/getting-started"}},l={},d=[{value:"Introduction",id:"introduction",level:2},{value:"Serializing Models &amp; Collections",id:"serializing-models-and-collections",level:2},{value:"Serializing To Vectors &amp; Maps",id:"serializing-to-vectors-and-maps",level:3},{value:"Serializing To JSON",id:"serializing-to-json",level:3},{value:"Relationships",id:"relationships",level:4},{value:"Hiding Attributes From JSON",id:"hiding-attributes-from-json",level:2},{value:"Temporarily Modifying Attribute Visibility",id:"temporarily-modifying-attribute-visibility",level:4},{value:"Appending Values To JSON",id:"appending-values-to-json",level:2},{value:"Appending At Run Time",id:"appending-at-run-time",level:4},{value:"Date Serialization",id:"date-serialization",level:2},{value:"Customizing The Default Date Format",id:"customizing-the-default-date-format",level:4},{value:"Customizing The Date Format Per Attribute",id:"customizing-the-date-format-per-attribute",level:4}],p={toc:d},u="wrapper";function m(e){let{components:t,...n}=e;return(0,a.kt)(u,(0,i.Z)({},p,n,{components:t,mdxType:"MDXLayout"}),(0,a.kt)("h1",{id:"tinyorm-serialization"},"TinyORM: Serialization"),(0,a.kt)("ul",null,(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#introduction"},"Introduction")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#serializing-models-and-collections"},"Serializing Models & Collections"),(0,a.kt)("ul",{parentName:"li"},(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#serializing-to-vectors-and-maps"},"Serializing To Vectors & Maps")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#serializing-to-json"},"Serializing To JSON")))),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#hiding-attributes-from-json"},"Hiding Attributes From JSON")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#appending-values-to-json"},"Appending Values To JSON")),(0,a.kt)("li",{parentName:"ul"},(0,a.kt)("a",{parentName:"li",href:"#date-serialization"},"Date Serialization"))),(0,a.kt)("h2",{id:"introduction"},"Introduction"),(0,a.kt)("p",null,"When building APIs using TinyORM, you will often need to convert your models and relationships to vectors, maps, or JSON. TinyORM includes convenient methods for making these conversions, as well as controlling which attributes are included in the serialized representation of your models."),(0,a.kt)("h2",{id:"serializing-models-and-collections"},"Serializing Models & Collections"),(0,a.kt)("h3",{id:"serializing-to-vectors-and-maps"},"Serializing To Vectors & Maps"),(0,a.kt)("p",null,"To convert a model and its loaded ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/relationships"},"relationships")," to a vector, you should use the ",(0,a.kt)("inlineCode",{parentName:"p"},"toVector")," or ",(0,a.kt)("inlineCode",{parentName:"p"},"toMap")," methods. This methods are recursive, so all attributes and all relations (including the relations of relations) will be converted to vectors:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'using Models::User;\n\nauto user = User::with("roles")->first();\n\nreturn user->toVector();\n\nreturn user->toMap();\n')),(0,a.kt)("p",null,"The ",(0,a.kt)("inlineCode",{parentName:"p"},"attributesToVector")," or ",(0,a.kt)("inlineCode",{parentName:"p"},"attributesToMap")," methods may be used to convert a model's attributes to a vector or map but not its relationships:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},"auto user = User::first();\n\nreturn user->attributesToVector();\n\nreturn user->attributesToMap();\n")),(0,a.kt)("p",null,"You may also convert entire ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections"},"collections")," of models to vectors or maps by calling the ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections#method-tovector"},(0,a.kt)("inlineCode",{parentName:"a"},"toVector"))," or ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections#method-tomap"},(0,a.kt)("inlineCode",{parentName:"a"},"toMap"))," methods on the collection instance:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'ModelsCollection<User> users = User::with("roles")->all();\n\nreturn users.toVector();\n\nreturn users.toMap();\n')),(0,a.kt)("h3",{id:"serializing-to-json"},"Serializing To JSON"),(0,a.kt)("p",null,"To convert a model to JSON, you should use the ",(0,a.kt)("inlineCode",{parentName:"p"},"toJson")," method. Like ",(0,a.kt)("inlineCode",{parentName:"p"},"toVector")," or ",(0,a.kt)("inlineCode",{parentName:"p"},"toMap"),", the ",(0,a.kt)("inlineCode",{parentName:"p"},"toJson")," method is recursive, so all attributes and relations will be converted to JSON. You may also specify any JSON encoding options that are supported by ",(0,a.kt)("a",{parentName:"p",href:"https://doc.qt.io/qt/qjsondocument.html#toJson"},"QJsonDocument::toJson"),":"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'using Models::User;\n\nauto user = User::with("roles")->find(1);\n\nreturn user->toJson();\n\nreturn user->toJson(QJsonDocument::Indented);\n')),(0,a.kt)("p",null,"You may also convert entire ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections"},"collections")," of models to JSON by calling the ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections#method-tojson"},(0,a.kt)("inlineCode",{parentName:"a"},"toJson"))," method on the collection instance:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'ModelsCollection<User> users = User::with("roles")->findMany({1, 2});\n\nreturn users.toJson();\n')),(0,a.kt)("p",null,"You can also convert models to the ",(0,a.kt)("a",{parentName:"p",href:"https://doc.qt.io/qt/qjsonobject.html"},(0,a.kt)("inlineCode",{parentName:"a"},"QJsonObject"))," and ",(0,a.kt)("a",{parentName:"p",href:"https://doc.qt.io/qt/qjsondocument.html"},(0,a.kt)("inlineCode",{parentName:"a"},"QJsonDocument"))," using the ",(0,a.kt)("inlineCode",{parentName:"p"},"toJsonArray")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"toJsonDocument")," methods and collection of models to ",(0,a.kt)("a",{parentName:"p",href:"https://doc.qt.io/qt/qjsonarray.html"},(0,a.kt)("inlineCode",{parentName:"a"},"QJsonArray"))," and ",(0,a.kt)("a",{parentName:"p",href:"https://doc.qt.io/qt/qjsondocument.html"},(0,a.kt)("inlineCode",{parentName:"a"},"QJsonDocument"))," using the ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections#method-tojsonarray"},(0,a.kt)("inlineCode",{parentName:"a"},"toJsonArray"))," and ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/collections#method-tojsondocument"},(0,a.kt)("inlineCode",{parentName:"a"},"toJsonDocument"))," methods."),(0,a.kt)("h4",{id:"relationships"},"Relationships"),(0,a.kt)("p",null,'When a TinyORM model is converted to JSON, its loaded relationships will automatically be included as attributes on the JSON object. Also, though TinyORM relationship methods are defined using "camelCase" method names, a relationship\'s JSON attributes will be "snake_case".'),(0,a.kt)("p",null,"This behavior is affected and can be overridden by the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_snakeAttributes")," static data member:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},"#include <orm/tiny/model.hpp>\n\nusing Orm::Tiny::Model;\n\nclass Album final : public Model<Album, AlbumImage>\n{\n    friend Model;\n    using Model::Model;\n\n    /*! Indicates whether attributes are snake_cased during serialization. */\n    inline static const bool u_snakeAttributes = false;\n};\n")),(0,a.kt)("h2",{id:"hiding-attributes-from-json"},"Hiding Attributes From JSON"),(0,a.kt)("p",null,"Sometimes you may wish to limit the attributes, such as passwords, that are included in your model's vector, map, or JSON representation. To do so, add a ",(0,a.kt)("inlineCode",{parentName:"p"},"u_hidden")," static data member to your model. Attributes that are listed in the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_hidden")," data member set will not be included in the serialized representation of your model:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'#include <orm/tiny/model.hpp>\n\nusing Orm::Tiny::Model;\n\nclass User final : public Model<User>\n{\n    friend Model;\n    using Model::Model;\n\n    /*! The attributes that should be hidden during serialization. */\n    inline static std::set<QString> u_hidden {"password"};\n};\n')),(0,a.kt)("admonition",{type:"note"},(0,a.kt)("p",{parentName:"admonition"},"To hide relationships, add the relationship's method name to your TinyORM model's ",(0,a.kt)("inlineCode",{parentName:"p"},"u_hidden")," static data member.")),(0,a.kt)("p",null,"Alternatively, you may use the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_visible"),' static data member to define an "allow list" of attributes that should be included in your model\'s vector, map, and JSON representation. All attributes that are not present in the ',(0,a.kt)("inlineCode",{parentName:"p"},"u_visible")," set will be hidden during serialization:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'#include <orm/tiny/model.hpp>\n\nusing Orm::Tiny::Model;\n\nclass User final : public Model<User>\n{\n    friend Model;\n    using Model::Model;\n\n    /*! The attributes that should be visible during serialization. */\n    inline static std::set<QString> u_visible {\n        "first_name", "last_name",\n    };\n};\n')),(0,a.kt)("h4",{id:"temporarily-modifying-attribute-visibility"},"Temporarily Modifying Attribute Visibility"),(0,a.kt)("p",null,"If you would like to make some typically hidden attributes visible on a given model instance, you may use the ",(0,a.kt)("inlineCode",{parentName:"p"},"makeVisible")," method. The ",(0,a.kt)("inlineCode",{parentName:"p"},"makeVisible")," method returns a model reference:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'return user.makeVisible("attribute").toMap();\n\nreturn user.makeVisible({"id", "name"}).toMap();\n')),(0,a.kt)("p",null,"Likewise, if you would like to hide some attributes that are typically visible, you may use the ",(0,a.kt)("inlineCode",{parentName:"p"},"makeHidden")," method."),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'return user.makeHidden("attribute").toVector();\n\nreturn user.makeHidden({"id", "name"}).toVector();\n')),(0,a.kt)("p",null,"If you wish to temporarily override all of the visible or hidden attributes, you may use the ",(0,a.kt)("inlineCode",{parentName:"p"},"setVisible")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"setHidden")," methods respectively:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'return user.setVisible({"id", "name"}).toMap();\n\nreturn user.setHidden({"email", "password", "note"}).toJson();\n')),(0,a.kt)("p",null,"You can also clear all visible and hidden attributes or determine whether a visible / hidden attribute is defined:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'user.clearVisible();\n\nuser.clearHidden();\n\nreturn user.hasVisible("name");\n\nreturn user.hasHidden("password");\n')),(0,a.kt)("h2",{id:"appending-values-to-json"},"Appending Values To JSON"),(0,a.kt)("p",null,"Occasionally, when converting models to vector, map, or JSON, you may wish to add attributes that do not have a corresponding column in your database. To do so, first define an ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/casts#accessors"},"accessor")," for the value:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'#include <orm/tiny/model.hpp>\n\nusing Orm::Tiny::Model;\n\nclass User final : public Model<User>\n{\n    friend Model;\n    using Model::Model;\n\nprotected:\n    /*! Accessor to determine if the user is an administrator. */\n    Attribute isAdmin() const noexcept\n    {\n        return Attribute::make(/* get */ []() -> QVariant\n        {\n            return QStringLiteral("yes");\n        });\n    }\n};\n')),(0,a.kt)("p",null,"If you would like the accessor to always be appended to your model's vector, map, and JSON representations, you may add the attribute name to the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_appends"),' data member set of your model. Note that attribute names are typically referenced using their "snake_case" serialized representation, even though the accessor\'s method name is defined using "camelCase":'),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'#include <orm/tiny/model.hpp>\n\nusing Orm::Tiny::Model;\n\nclass User final : public Model<User>\n{\n    friend Model;\n    using Model::Model;\n\n    /*! Map of mutator names to methods. */\n    inline static const QHash<QString, MutatorFunction> u_mutators {\n        {"is_admin", &User::isAdmin},\n    };\n\n    /*! The attributes that should be appended during serialization. */\n    std::set<QString> u_appends {"is_admin"};\n};\n')),(0,a.kt)("p",null,"Once the attribute has been added to the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_appends")," set, it will be included in both the model's vector, map, and JSON representations. Attributes in the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_appends")," set will also respect the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_visible")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"u_hidden")," attribute settings configured on the model."),(0,a.kt)("p",null,"Special note should be given to the ",(0,a.kt)("inlineCode",{parentName:"p"},"u_mutators")," static data member map, which maps accessors' attribute names to its methods. This data member is ",(0,a.kt)("strong",{parentName:"p"},"required")," because C++ does not currently support reflection."),(0,a.kt)("h4",{id:"appending-at-run-time"},"Appending At Run Time"),(0,a.kt)("p",null,"At runtime, you may instruct a model instance to append additional attributes using the ",(0,a.kt)("inlineCode",{parentName:"p"},"append")," method. Or, you may use the ",(0,a.kt)("inlineCode",{parentName:"p"},"setAppends")," method to override the entire set of appended attributes for a given model instance:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'return user.append("is_admin").toVector();\n\nreturn user.append({"is_admin", "is_banned"}).toMap();\n\nreturn user.setAppends({"is_admin"}).toJson();\n')),(0,a.kt)("p",null,"And you can also clear all appends or determine whether an append is defined:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'user.clearAppends();\n\nreturn user.hasAppend("is_admin");\n')),(0,a.kt)("h2",{id:"date-serialization"},"Date Serialization"),(0,a.kt)("h4",{id:"customizing-the-default-date-format"},"Customizing The Default Date Format"),(0,a.kt)("p",null,"You may customize the default serialization format by overriding the ",(0,a.kt)("inlineCode",{parentName:"p"},"serializeDate")," and ",(0,a.kt)("inlineCode",{parentName:"p"},"serializeDateTime")," methods. These methods do not affect how your dates are formatted for storage in the database:"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'/*! Prepare a date for vector, map, or JSON serialization. */\nQString serializeDate(const QDate date)\n{\n    return date.toString("yyyy-MM-dd");\n}\n\n/*! Prepare a datetime for vector, map, or JSON serialization. */\nQString serializeDateTime(const QDateTime &datetime)\n{\n    return datetime.toUTC().toString("yyyy-MM-ddTHH:mm:ssZ");\n}\n')),(0,a.kt)("h4",{id:"customizing-the-date-format-per-attribute"},"Customizing The Date Format Per Attribute"),(0,a.kt)("p",null,"You may customize the serialization format of individual TinyORM date attributes by specifying the date format in the model's ",(0,a.kt)("a",{parentName:"p",href:"/tinyorm/casts#attribute-casting"},"cast declarations"),":"),(0,a.kt)("pre",null,(0,a.kt)("code",{parentName:"pre"},'/*! The attributes that should be cast. */\ninline static std::unordered_map<QString, CastItem> u_casts {\n    {"birthday",  {CastType::CustomQDate, "yyyy-MM-dd"}},\n    {"joined_at", {CastType::CustomQDateTime, "yyyy-MM-dd HH:00"}},\n};\n')))}m.isMDXComponent=!0}}]);