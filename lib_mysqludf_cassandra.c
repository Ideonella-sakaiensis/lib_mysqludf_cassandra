#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include <mysql.h>

#include <cjson/cJSON.h>
#include <cassandra.h>




static cJSON*
getJsonReply(CassFuture *future) {
	/* This will block until the query has finished */
	CassError rc = cass_future_error_code(future);

	cJSON *result = NULL;
	if (rc != CASS_OK) {
		const char *message;
		size_t message_length;
		cass_future_error_message(future, &message, &message_length);

		result = cJSON_CreateObject();
		cJSON_AddNumberToObject(result, "result_code", rc);
		cJSON_AddStringToObject(result, "message", message);
	} else {
		result = cJSON_CreateObject();
		cJSON_AddNumberToObject(result, "result_code", rc);
	}
	return result;
}

static char*
getResultFromReply(CassFuture *future) {
	char *result;
	cJSON *json = getJsonReply(future);
	if (json != NULL) {
		result = cJSON_Print(json);
	}
	cJSON_Delete(json);
	return result;
}

static char*
execute_query(CassSession *session, const char* query_string)
{
	char *result;
	CassStatement* statement = cass_statement_new(query_string, 0);

	CassFuture* query_future = cass_session_execute(session, statement);
	/* Statement objects can be freed immediately after being executed */
	cass_statement_free(statement);
	{
		result = getResultFromReply(query_future);
	}
	cass_future_free(query_future);

	return result;
}


/**********************************
  cassandra
  
  cassandra(server, query);
  
  RETURN json
*/

my_bool
cassandra_init(UDF_INIT *initid,
               UDF_ARGS *args,
               char     *message)
{
	if (args->arg_count < 2) {
		strcpy(message, "requires at last two arguments.");
		return EXIT_FAILURE;
	}
	if (args->args[0] == NULL || args->args[1] == NULL) {
		initid->ptr = NULL;
	} else {
		if (args->arg_type[0] != STRING_RESULT ||
		    args->arg_type[1] != STRING_RESULT) {
			strcpy(message, "invalid arguments.");
			return EXIT_FAILURE;
		}
	}
	
	initid->maybe_null = 1;
	return EXIT_SUCCESS;
}


void
cassandra_deinit(UDF_INIT *initid)
{
	if (initid->ptr != NULL) {
		free(initid->ptr);
	}
}


char*
cassandra(UDF_INIT      *initid,
          UDF_ARGS      *args,
          char          *result,
          unsigned long *length,
          char          *is_null,
          char          *error)
{
	// check arguments
	if (args->args[0] == NULL || args->args[1] == NULL) {
		*is_null = 1;
		return NULL;
	} else {
		if (args->arg_type[0] != STRING_RESULT ||
		    args->arg_type[1] != STRING_RESULT) {
			// invalid arguments
			*error   = 1;
			return NULL;
		}
	}

	CassFuture *future = NULL;
	CassCluster *cluster = cass_cluster_new();
	CassSession *session = cass_session_new();

	char const *hosts        = args->args[0];
	char const *query_string = args->args[1];

	// connection to redis
	cass_cluster_set_contact_points(cluster, hosts);
	future = cass_session_connect(session, cluster);

	CassError rc = cass_future_error_code(future);
	if (rc != CASS_OK) {
		result = getResultFromReply(future);
		goto final;
	}

	// execute query
	result = execute_query(session, query_string);

final:
	if (future  != NULL) cass_future_free(future);
	if (session != NULL) cass_session_free(session);
	if (cluster != NULL) cass_cluster_free(cluster);

	if (result != NULL) {
		*length = strlen(result);
		initid->max_length = *length;
		initid->ptr = result;
	} else {
		*is_null = 1;
	}
	return result;
}
