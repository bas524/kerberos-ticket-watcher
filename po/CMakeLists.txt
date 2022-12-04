file(GLOB KRB5TW_PO_FILES ${CMAKE_SOURCE_DIR}/po/*.po)
gettext_create_krb5tw_translations("${CMAKE_SOURCE_DIR}/po/krb5-ticket-watcher.pot" "ALL" ${KRB5TW_PO_FILES})
