#ifndef CGI_SESS_STDTYPE_H
#define CGI_SESS_STDTYPE_H

#include "cgi_session.h"

/** Register int type session variable.
 * Registers variable with int type serializer/deserializer.
 */
#define SESSION_INT(ctx, var)   \
        SESSION_REGISTER(ctx, var, serialize_int, deserialize_int)
void serialize_int(struct s_serialized *, void *var);
void deserialize_int(struct s_serialized *, void *var);

/** Register char* type session variable.
 * Registers variable with char* type serializer/deserializer.
 */
#define SESSION_STRING(ctx, var) \
        SESSION_REGISTER(ctx, var, serialize_string, deserialize_string)
void serialize_string(struct s_serialized *, void *var);
void deserialize_string(struct s_serialized *, void *var);

/** Register long type session variable.
 * Registers variable with long type serializer/deserializer.
 */
#define SESSION_LONG(ctx, var)  \
        SESSION_REGISTER(ctx, var, serialize_long, deserialize_long)
void serialize_long(struct s_serialized *, void *var);
void deserialize_long(struct s_serialized *, void *var);

/** Register float type session variable.
 * Registers variable with float type serializer/deserializer.
 */
#define SESSION_FLOAT(ctx, var) \
        SESSION_REGISTER(ctx, var, serialize_float, deserialize_float)
void serialize_float(struct s_serialized *, void *var);
void deserialize_float(struct s_serialized *, void *var);

/** Register double type session variable.
 * Registers variable with double type serializer/deserializer.
 */
#define SESSION_DOUBLE(ctx, var) \
        SESSION_REGISTER(ctx, var, serialize_double, deserialize_double)
void serialize_double(struct s_serialized *, void *var);
void deserialize_double(struct s_serialized *, void *var);

#endif