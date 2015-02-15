/* Include the required headers from httpd */
#include <httpd.h>
#include <http_core.h>
#include <http_protocol.h>
#include <http_request.h>
#include "apr_base64.h"
#include "apr_strings.h"

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int privcon_handler(request_rec *r);
static void decodeUrlSafeString(char string[]);

/* Define our module as an entity and assign a function for registering hooks  */

module AP_MODULE_DECLARE_DATA   mod_auth_privcon_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            // Per-directory configuration handler
    NULL,            // Merge handler for per-directory configurations
    NULL,            // Per-server configuration handler
    NULL,            // Merge handler for per-server configurations
    NULL,            // Any directives we may have for httpd
    register_hooks   // Our hook registering function
};

struct Policy
{
    char b64str[1024];
    char url[1024];
    char client_ip[15];
    char dateLessThan[1024];
    char dateGreaterThan[1024];
    char signiture[1024]; 
};

/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{
    
    /* Hook the request handler */
    ap_hook_handler(privcon_handler, NULL, NULL, APR_HOOK_LAST);
}

/* The handler function for our module.
 * This is where all the fun happens!
 */

static int privcon_handler(request_rec *r)
{
    /* First off, we need to check if this is a call for the "example" handler.
     * If it is, we accept it and do our things, it not, we simply return DECLINED,
     * and Apache will try somewhere else.
     */
    if (!r->handler || strcmp(r->handler, "privcon-handler")) return (DECLINED);
    
    // The first thing we will do is write a simple "Hello, world!" back to the client.
    ap_rputs("Hello, world!<br/>", r);
    ap_rputs(r->useragent_ip, r);
    ap_rputs("\n", r);

    struct Policy policy;

    // Get the querystring parameters and populate the policy object
    char *a, *next, *last, *pnext, *plast;
    next = apr_strtok(r->args, "&", &last);

    while (next) {
        pnext = apr_strtok(next, "=", &plast);

        if (strcmp(pnext, "policy")==0) {
            pnext = apr_strtok(NULL, "=", &plast);
            strcpy(policy.b64str, pnext);
        } else if (strcmp(pnext, "signiture")==0) {
            pnext = apr_strtok(NULL, "=", &plast);
            strcpy(policy.signiture, pnext);
        }

        next = apr_strtok(NULL, "&", &last);
    }

    ap_rputs("\n", r);
    ap_rputs(policy.b64str, r);
    
    

    ap_rputs("\n", r);
    decodeUrlSafeString(policy.b64str);
    ap_rputs("\n", r);
    ap_rputs(policy.b64str, r);

    char decoded[1024];
    apr_base64_decode(decoded, policy.b64str);
    ap_rputs("\n", r);
    ap_rputs(decoded, r);

    return OK;
}


static void decodeUrlSafeString(char string[]) {
    const char safe[] = {'-', '_', '~'};
    const char unsafe[] = {'+', '=', '/'}; 

    for (int x = 0; x < strlen(string); x++) {
        for (int y = 0; y < sizeof(safe); y++) {
            if (string[x] == safe[y]) {
                string[x]=unsafe[y];
            }
        }
    }
}
