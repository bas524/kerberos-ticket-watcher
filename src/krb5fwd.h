//
// Created by Alexander Bychuk on 17.08.2022.
//

#ifndef KRB5_TICKET_WATCHER_SRC_KRB5FWD_H_
#define KRB5_TICKET_WATCHER_SRC_KRB5FWD_H_
#include <cstdint>

typedef int32_t krb5_int32;
typedef krb5_int32 krb5_error_code;
typedef krb5_int32 krb5_timestamp;
typedef krb5_int32 krb5_deltat;

struct _krb5_context;
typedef struct _krb5_context* krb5_context;

struct _krb5_creds;
typedef struct _krb5_creds krb5_creds;

struct _krb5_ticket;
typedef struct _krb5_ticket krb5_ticket;

struct _krb5_ccache;
typedef struct _krb5_ccache* krb5_ccache;

typedef void* krb5_pointer;
typedef krb5_pointer krb5_cc_cursor;

struct _krb5_cc_ops;
typedef struct _krb5_cc_ops krb5_cc_ops;

struct krb5_principal_data;
typedef struct krb5_principal_data* krb5_principal;

struct _krb5_get_init_creds_opt;
typedef struct _krb5_get_init_creds_opt krb5_get_init_creds_opt;

#endif  // KRB5_TICKET_WATCHER_SRC_KRB5FWD_H_
