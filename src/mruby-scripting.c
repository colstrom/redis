#include "server.h"
#include <mruby.h>
#include <mruby/string.h>

void mrbScriptingInit(int setup) {
  mrb_state *mrb = mrb_open();

  if (setup) server.mrb_client = NULL;

  if (server.mrb_client == NULL) {
    server.mrb_client = createClient(-1);
    server.mrb_client->flags |= CLIENT_LUA;
  }

  server.mrb = mrb;
}

void mrbScriptingRelease(void) {
  mrb_close(server.mrb);
}

void mrbScriptingReset(void) {
  mrbScriptingRelease();
  mrbScriptingInit(0);
}

void mrbAddReply(client *client, mrb_state *mrb, mrb_value value) {
  value = mrb_inspect(mrb, value);

  if (!mrb_string_p(value)) return;

  char *string = RSTRING_PTR(value);
  int   length = RSTRING_LEN(value);

  addReplyBulkCBuffer(client, string, length);
}
