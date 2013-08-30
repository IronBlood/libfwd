#include <stdlib.h>
#include <string.h>
#include <fwd/cgi_session.h>
#include <fwd/cgi_sess_stdtype.h>

// TODO: unchecked 20121004 by IronBlood

/** Serialize int type variable.
 * Converts int type variable to s_serialized structure.
 * @see t_serializer
 */
void serialize_int(struct s_serialized *s_val, void *var)
{
        int *value = var;

        s_val->size = sizeof(int);
        s_val->data = malloc(sizeof(int));
       
        if(s_val->data)
                *(int*)s_val->data = *value;
};

/** Serialize int type variable.
 * Converts s_serialized structure to int type.
 * @see t_deserializer
 */
void deserialize_int(struct s_serialized *s_val, void *var)
{
        int *value = var;

        if(s_val->data)
                *value = *(int*)s_val->data;
};


/** Serialize char* type variable.
 * Converts char* type variable to s_serialized structure.
 * @see t_serializer
 */
void serialize_string(struct s_serialized *s_val, void *var)
{
        char **value = var;

        if(*value)
        {
                s_val->size = strlen(*value) + 1;
                s_val->data = malloc(s_val->size);

                if(s_val->data)
                        memcpy(s_val->data, *value, s_val->size);
        }
        else
        {
                s_val->size = 1;
                s_val->data = malloc(s_val->size);

                if(s_val->data)
                        *(char*)s_val->data = 1;
        }
};

/** Serialize char* type variable.
 * Converts s_serialized structure to char* type.
 * @see t_deserializer
 */
void deserialize_string(struct s_serialized *s_val, void *var)
{
        char **value = var;

        if(s_val->size == 1)
        {
                if(*(char*)s_val->data == 1)
                {
                        *value = NULL;
                        return;
                }
        }

        if(s_val->data)
                *value = strdup((char*)s_val->data);
};


/** Serialize long type variable.
 * Converts long type variable to s_serialized structure.
 * @see t_serializer
 */
void serialize_long(struct s_serialized *s_val, void *var)
{
        long *value = var;

        s_val->size = sizeof(long);
        s_val->data = malloc(sizeof(long));
       
        if(s_val->data)
                *(long*)s_val->data = *value;
};

/** Serialize long type variable.
 * Converts s_serialized structure to long type.
 * @see t_deserializer
 */
void deserialize_long(struct s_serialized *s_val, void *var)
{
        long *value = var;

        if(s_val->data)
                *value = *(long*)s_val->data;
};


/** Serialize float type variable.
 * Converts float type variable to s_serialized structure.
 * @see t_serializer
 */
void serialize_float(struct s_serialized *s_val, void *var)
{
        float *value = var;

        s_val->size = sizeof(float);
        s_val->data = malloc(sizeof(float));
       
        if(s_val->data)
                *(float*)s_val->data = *value;
};

/** Serialize float type variable.
 * Converts s_serialized structure to float type.
 * @see t_deserializer
 */
void deserialize_float(struct s_serialized *s_val, void *var)
{
        float *value = var;

        if(s_val->data)
                *value = *(float*)s_val->data;
};


/** Serialize double type variable.
 * Converts double type variable to s_serialized structure.
 * @see t_serializer
 */
void serialize_double(struct s_serialized *s_val, void *var)
{
        double *value = var;

        s_val->size = sizeof(double);
        s_val->data = malloc(sizeof(double));
       
        if(s_val->data)
                *(double*)s_val->data = *value;
};

/** Serialize double type variable.
 * Converts s_serialized structure to double type.
 * @see t_deserializer
 */
void deserialize_double(struct s_serialized *s_val, void *var)
{
        double *value = var;

        if(s_val->data)
                *value = *(double*)s_val->data;
};
