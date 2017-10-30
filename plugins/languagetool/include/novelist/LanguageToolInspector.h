/**********************************************************
 * @file   LanguageToolInspector.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_LANGUAGETOOLINSPECTOR_H
#define NOVELIST_LANGUAGETOOLINSPECTOR_H

#include <widgets/texteditor/Inspector.h>

namespace novelist {
    /**
     * Forwards inspection requests to a local instance of LanguageTool
     */
    class LanguageToolInspector : public Inspector {
    public:
        InspectionBlockResult inspect(QString const& text, Language lang) const noexcept override;
    };
}

#endif //NOVELIST_LANGUAGETOOLINSPECTOR_H
