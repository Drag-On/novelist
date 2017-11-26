/**********************************************************
 * @file   SceneDocument.h
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SCENEDOCUMENT_H
#define NOVELIST_SCENEDOCUMENT_H

#include <QTextDocument>
#include <QTextFragment>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <memory>
#include "SceneDocumentInsightManager.h"
#include "model/Language.h"

namespace novelist {

    class TextEditor;
    class InsightModel;
    class BaseInsight;

    /**
     * Formatted text for scenes
     */
    class NOVELIST_CORE_EXPORT SceneDocument : public QTextDocument {
    Q_OBJECT

    public:
        /**
         * Construct document
         * @param lang Document language
         * @param parent Parent object
         */
        explicit SceneDocument(Language lang, QObject* parent = nullptr);

        /**
         * Construct document and initialize it with a text
         * @param text Initial text
         * @param lang Document language
         * @param parent Parent object
         */
        explicit SceneDocument(QString text, Language lang, QObject* parent = nullptr);

        /**
         * Read the scene from file
         * @param file File to read from
         * @return true in case of success, otherwise false
         */
        bool read(QFile& file);

        /**
         * Read the scene from xml definition
         * @param xml XML code
         * @return true in case of success, otherwise false
         */
        bool read(QString const& xml);

        /**
         * Write the scene to file
         * @param file File to write to
         * @return true in case of success, otherwise false
         */
        bool write(QFile& file) const;

        /**
         * Write the scene to a string
         * @param[out] xml String to write to
         * @return true in case of success, otherwise false
         */
        bool write(QString& xml) const;

        /**
         * @return Document language
         */
        Language language() const noexcept;

        /**
         * @param lang New document language
         */
        void setLanguage(Language lang) noexcept;

        /**
         * Compares two documents for content-equality
         * @details This only considers text. Formatting is not considered.
         * @param other Document to compare to
         * @return True in case both documents are equal, otherwise false
         */
        bool operator==(SceneDocument const& other) const;

        /**
         * Compares two documents for content-inequality
         * @details This only considers text. Formatting is not considered.
         * @param other Document to compare to
         * @return True in case both documents are not equal, otherwise false
         */
        bool operator!=(SceneDocument const& other) const;

    private:
        SceneDocumentInsightManager m_insightMgr;
        Language m_lang;

        bool readInternal(QXmlStreamReader& xml);

        bool readBlock(QXmlStreamReader& xml, QTextCursor& cursor);

        bool readText(QXmlStreamReader& xml, QTextCursor& cursor);

        QTextBlockFormat readBlockFormatAttr(QXmlStreamReader& xml) const;

        QTextCharFormat readCharFormatAttr(QXmlStreamReader& xml) const;

        bool writeBlock(QXmlStreamWriter& xml, QTextBlock const& block) const;

        bool writeFragment(QXmlStreamWriter& xml, QTextFragment const& fragment) const;

        bool writeBlockFormatAttr(QXmlStreamWriter& xml, QTextBlockFormat const& format) const;

        bool writeCharFormatAttr(QXmlStreamWriter& xml, QTextCharFormat const& format) const;

        /**
         * Provides access to this document's insight manager
         * @return Reference to the insight manager
         */
        SceneDocumentInsightManager& insightManager();

        friend TextEditor;
        friend InsightModel;
        friend BaseInsight;
    };
}

#endif //NOVELIST_SCENEDOCUMENT_H
