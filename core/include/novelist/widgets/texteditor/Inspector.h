/**********************************************************
 * @file   Inspector.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSPECTOR_H
#define NOVELIST_INSPECTOR_H

#include "model/Language.h"
#include "TextEditorInsightManager.h"

namespace novelist {
    /**
     * Interface class for inspectors to implement
     */
    class Inspector {
    public:
        virtual ~Inspector() noexcept = default;

        /**
         * Inspects a text block
         * @param text Text to inspect
         * @param lang Text language
         * @return Resulting insights
         */
        virtual InspectionBlockResult inspect(QString const& text, Language lang) const noexcept = 0;
    };
}

#endif //NOVELIST_INSPECTOR_H
