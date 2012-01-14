/* 
    jack_capture - OSC remote control

    Kjetil Matheussen, 2005-2010.
    Written 2012 by Robin Gareus <robin@gareus.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_LIBLO

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <lo/lo.h>

#include <stdbool.h>
extern bool verbose;
extern bool silent;

/* message flags */
extern bool queued_file_rotate;
void osc_stop();

/***************************************************************************
 * specific OSC callbacks */

int oscb_record (const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data){
  if (verbose==true) fprintf(stderr, "OSC: %s <- s:%s\n", path, &argv[0]->s);
  // TODO ?? akin use_jack_transport jack_transport_started
  return(0);
}

int oscb_stop (const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data){
  if (verbose==true) fprintf(stderr, "OSC: %s\n", path);
  osc_stop();
  return(0);
}

int oscb_frotate (const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data){
  if (verbose==true) fprintf(stderr, "OSC: %s\n", path);
  queued_file_rotate=true;
  return(0);
}

#if 0
int oscb_offset (const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data){
  if (verbose==true) fprintf(stderr, "OSC: %s <- f:%f\n", path, argv[0]->f);
  if (verbose==true) fprintf(stderr, "OSC: %s <- i:%i\n", path, argv[1]->i);
  return(0);
}
#endif


/***************************************************************************
 * general callbacks */
int oscb_quit (const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data){
  fprintf(stderr, "OSC 'quit' command recv.\n");
  //loop_flag=0;
  return(0);
}

static void oscb_error(int num, const char *m, const char *path) {
  fprintf(stderr, "liblo server error %d in path %s: %s\n", num, path, m);
}

/* ///////////////////////////////////////////////////////////////////////// */
/* main OSC server code                                                      */

lo_server_thread osc_server = NULL;

int init_osc(int osc_port) {
  char tmp[8];
  if (osc_port < 0) return(1);
  uint32_t port = (osc_port>100 && osc_port< 60000)?osc_port:7654;

  snprintf(tmp, sizeof(tmp), "%d", port);
  if(verbose==true)
    fprintf(stderr, "OSC trying port:%i\n",port);
  osc_server = lo_server_thread_new (tmp, oscb_error);

  if (!osc_server) {
    if(silent==false) fprintf(stderr, "OSC start failed.");
    return(1);
  }

  if(silent==false) {
    char *urlstr;
    urlstr = lo_server_thread_get_url (osc_server);
    fprintf(stderr, "OSC server name: %s\n",urlstr);
    free (urlstr);
  }

  //lo_server_thread_add_method(osc_server, "/jack_capture/start",  "s", &oscb_record, NULL); 
  lo_server_thread_add_method(osc_server, "/jack_capture/stop",   "",  &oscb_stop, NULL); 
  lo_server_thread_add_method(osc_server, "/jack_capture/rotate", "",  &oscb_frotate, NULL); 

  lo_server_thread_start(osc_server);
  if(verbose==true) fprintf(stderr, "OSC server started on port %i\n",port);
  return (0);
}

void shutdown_osc(void) {
  if (!osc_server) return;
  lo_server_thread_stop(osc_server);
  if(verbose==true) fprintf(stderr, "OSC server shut down.\n");
}

#else
int init_osc(int osc_port) {return(1);}
void shutdown_osc(void) {;}
#endif

/* vi:set ts=8 sts=2 sw=2: */
