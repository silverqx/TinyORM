TEMPLATE = subdirs

SUBDIRS = \
    castattributes \
    collection_models \
    collection_relations \
    model \
    model_appends \
    model_conn_indep \
    model_hidesattributes \
    model_qdatetime \
    model_relations \
    model_return_relation \
    model_serialization \
    queriesrelationships \
    relations_buildsqueries \
    relations_conn_indep \
    relations_insrt_updt \
    softdeletes \
    tinybuilder \

model_hidesattributes.depends = model_serialization
model_appends.depends = model_serialization model_hidesattributes
