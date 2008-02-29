#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oggz-chop.h"
#include "header.h"
#include "httpdate.h"

static void
set_param (OCState * state, char * key, char * val)
{
  if (!strncmp ("s", key, 2)) state->start = atof(val);
  if (!strncmp ("start", key, 6)) state->start = atof(val);

  if (!strncmp ("e", key, 2)) state->end = atof(val);
  if (!strncmp ("end", key, 6)) state->end = atof(val);
}

/**
 * Parse the name=value pairs in the query string and set parameters
 * @param start,end The range parameters to set
 * @param query The query string
 */
static void
parse_query (OCState * state, char * query)
{
  char * key, * val, * end;

  if (!query) return;

  key = query;

  do {
    val = strchr (key, '=');
    end = strchr (key, '&');

    if (end) {
      if (val) {
        if (val < end) {
          *val++ = '\0';
        } else {
          val = NULL;
        }
      }
      *end++ = '\0';
    } else {
      if (val) *val++ = '\0';
    }

    /* fprintf (stderr, "%s = %s\n", key, val);*/
    set_param (state, key, val);

    key = end;

  } while (end != NULL);

  return;
}

int
cgi_test (void)
{
  char * gateway_interface;

  gateway_interface = getenv ("GATEWAY_INTERFACE");
  if (gateway_interface == NULL) {
    return 0;
  }

  return 1;
}

#if 0
static int
cgi_send_photo (photo_t * photo)
{
  /*header_content_length (photo->size);*/
  header_end();

  photo_put (photo);

  return 0;
}

static int
cgi_send (OCState * state)
{
  header_last_modified (state->in.mtime);

  if (params->nochange) {
    cgi_send_photo (&params->in);
  } else if (params->out.name) {
    cgi_send_photo (&params->out);
  } else {
    header_content_length ((off_t)params->data_size);
    header_end();

    memory_send (params);
  }

  return 0;
}
#endif

int
cgi_main (OCState * state)
{
  int err = 0;
  char * path_info;
  char * path_translated;
  char * query_string;
  char * if_modified_since;
  time_t since_time;

  httpdate_init ();

  path_info = getenv ("PATH_INFO");
  path_translated = getenv ("PATH_TRANSLATED");
  query_string = getenv ("QUERY_STRING");
  if_modified_since = getenv ("HTTP_IF_MODIFIED_SINCE");

  state->infilename = path_translated;
  state->outfilename = NULL;
  state->start = 0.0;
  state->end = -1.0;

  /*photo_init (&params->in, path_translated);*/

#if 0
  if (if_modified_since != NULL) {
    int len;

    fprintf (stderr, "If-Modified-Since: %s\n", if_modified_since);

    len = strlen (if_modified_since) + 1;
    since_time = httpdate_parse (if_modified_since, len);

    if (state->in.mtime <= since_time) {
      header_not_modified();
      header_end();
      return 1;
    }
  }
#endif

  header_content_type_ogg ();

  /*config_init (params);*/

  parse_query (state, query_string);

#if 0
  if (params->x || params->y || params->scale || params->gray ||
      params->quality) {
    cache_init (params, path_info);
  } else {
    params->nochange = 1;
  }
#endif

  header_end();

  err = 0;
#if 0
  if (!(params->nochange || params->cached)) {
    err = chop (state);
  }
#else
  err = chop (state);
#endif
  
  return err;
}
