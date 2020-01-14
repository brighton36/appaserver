#include <stdlib.h>
#include <string>
#include <iostream>
#include <ctemplate/template.h>
#include "ctemplate_wrapper.h"

extern "C" {

  void* newCTemplate(char *szName) {
    std::string sTmplname(szName);

    ctemplate::TemplateDictionary* dict = new ctemplate::TemplateDictionary(sTmplname);

    return dict;
  }

  void deleteCTemplate(void *v) {
    ctemplate::TemplateDictionary *dict = static_cast<ctemplate::TemplateDictionary *>(v);

    delete dict;
	}

  void CTemplate_SetValue(void* v, char *szName, char *szValue) {
    ctemplate::TemplateDictionary *dict = static_cast<ctemplate::TemplateDictionary *>(v);

    std::string sName(szName);
    std::string sValue(szValue);

		dict->SetValue(sName, sValue);
  }

  void CTemplate_SetIntValue(void* v, char *szName, long iValue) {
    ctemplate::TemplateDictionary *dict = static_cast<ctemplate::TemplateDictionary *>(v);

    std::string sName(szName);

		dict->SetIntValue(sName, iValue);
  }

  void CTemplate_Render(void *v) {
    ctemplate::TemplateDictionary *dict = static_cast<ctemplate::TemplateDictionary *>(v);

    std::string sPathViews(PATH_VIEWS);
    std::string sTemplateExt(TEMPLATE_EXT);

		std::string output;
    std::string sTemplatePath;

    sTemplatePath = sPathViews+dict->name()+sTemplateExt;

		ctemplate::ExpandTemplate(sTemplatePath, ctemplate::DO_NOT_STRIP, dict, &output);
		std::cout << output;
		return;
  }
}

