#ifndef __CTEMPLATE_WRAPPER_H
#define __CTEMPLATE_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: This is probably better off in the site config file...
#define PATH_VIEWS "/var/appaserver/views/"
#define TEMPLATE_EXT ".tpl"

typedef struct CTemplate CTemplate;

void* newCTemplate(char *szName);
void deleteCTemplate(void *v);

void CTemplate_SetValue(void* v, char *szName, char *szValue);
void CTemplate_SetIntValue(void* v, char *szName, long iValue);

void CTemplate_Render(void* v);

#ifdef __cplusplus
}
#endif
#endif
