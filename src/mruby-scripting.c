#include "server.h"
#include <mruby.h>
#include <mruby/string.h>
#include <mruby/proc.h>

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

void mrbEvalCommand(client *client) {
  mrbScriptingReset();

         mrb_state        *mrb     = server.mrb;
         mrbc_context     *context = mrbc_context_new(mrb);
  struct mrb_parser_state *parser  = mrb_parse_string(mrb, client->argv[1]->ptr, context);
  struct RProc            *proc    = mrb_generate_code(mrb, parser); /* TODO: Should be able to cache this... */
         mrb_value         value   = mrb_run(mrb, proc, mrb_top_self(mrb));

  mrbAddReply(client, mrb, value);

  /* Cleanup */
  mrb_pool_close(parser->pool);
  mrbc_context_free(mrb, context);
}
